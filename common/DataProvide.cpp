#include "DataProvide.hpp"
#include "Data.hpp"

using namespace std;
using namespace cv;

const Scalar default_color(127, 127, 127);

TileProvider::TileProvider(std::string path, std::string info_file) {
	this->path = path;
	m_scene_data = make_shared<SceneData>();
	m_scene_data->load(info_file);

	m_pixel_rows.resize(m_scene_data->getNumLayers());
	m_pixel_cols.resize(m_scene_data->getNumLayers());

	int top_id = m_scene_data->getNumLayers() - 1;
	int top_rows = m_scene_data->getRowsOfLayer(top_id) * m_scene_data->getTileLen();
	int top_cols = m_scene_data->getColsOfLayer(top_id) * m_scene_data->getTileLen();

	m_pixel_rows[top_id] = top_rows;
	m_pixel_cols[top_id] = top_cols;

	for (int i = top_id - 1; i >= 0; --i ) {
		m_pixel_rows[i] = m_pixel_rows[i + 1] / 2;
		m_pixel_cols[i] = m_pixel_cols[i + 1] / 2;
	}
	// for (size_t i = 0; i < m_pixel_cols.size(); ++i) {
	// 	cout << m_pixel_rows[i] << "\t" << m_pixel_cols[i] << endl;
	// }
}
int TileProvider::getTileLen() {
	return m_scene_data->getTileLen();
}
int TileProvider::getNumLayers() {
	return m_scene_data->getNumLayers();
}
int TileProvider::getRowsOfLayer(int layer_id) {
	return m_scene_data->getRowsOfLayer(layer_id);
}
int TileProvider::getColsOfLayer(int layer_id) {
	return m_scene_data->getColsOfLayer(layer_id);
}
int TileProvider::getPixelRowsOfLayer(int layer_id) {
	assert(layer_id >= 0 && layer_id < getNumLayers());
	return m_pixel_rows[layer_id];
}
int TileProvider::getPixelColsOfLayer(int layer_id) {
	assert(layer_id >= 0 && layer_id < getNumLayers());
	return m_pixel_cols[layer_id];
}
cv::Mat TileProvider::getTile(int x, int y, int z) {
	cv::Mat tile = imread(path + m_scene_data->getTileName(x, y, z));
	// cout << m_scene_data->getTileName(x, y, z) << endl;
	assert(!tile.empty());
	if (tile.rows < getTileLen() || tile.cols < getTileLen()) {
		Mat tmp(getTileLen(), getTileLen(), tile.type());
		tmp.setTo(default_color);
		Rect rect(0, 0, tile.cols, tile.rows);
		tile.copyTo(tmp(rect));
		tile = tmp;
	}
	return tile;
}
SceneFrameProvider::SceneFrameProvider(std::string path, std::string info_file, int w, int h) {
	m_tile_provider = make_shared<TileProvider>(path, info_file);
	m_frame_width = w;
	m_frame_height = h;
}
cv::Mat SceneFrameProvider::getFrame(double x, double y, double z) {
	int layer_id = static_cast<int>(z);
	int layer_num = m_tile_provider->getNumLayers();
	layer_id = max(layer_id, 0);
	layer_id = min(layer_id, layer_num - 1);

	int pixel_x = static_cast<int>(x * m_tile_provider->getPixelColsOfLayer(layer_id));
	int pixel_y = static_cast<int>(y * m_tile_provider->getPixelRowsOfLayer(layer_id));
	pixel_x = max(pixel_x, 0);
	pixel_x = min(pixel_x, m_tile_provider->getPixelColsOfLayer(layer_id));
	pixel_y = max(pixel_y, 0);
	pixel_y = min(pixel_y, m_tile_provider->getPixelRowsOfLayer(layer_id));

	// pixel_x = 8896;
	// pixel_y = 960;
	// layer_id = 5;

	pixel_x -= m_frame_width / 2;
	pixel_y -= m_frame_height / 2;

	return getFrame(m_frame_width, m_frame_height, pixel_x, pixel_y, layer_id);
}
cv::Mat SceneFrameProvider::getFrame(int w, int h, int x, int y, int z) {
	const int LEN = m_tile_provider->getTileLen();

	Rect rect(x, y, w, h);
	Mat result(h, w, CV_8UC3);
	result.setTo(default_color);//default color

	int X1 = x / LEN;
	int Y1 = y / LEN;
	int X2 = (x + w - 1) / LEN;
	int Y2 = (y + h - 1) / LEN;

	Mat tile;
	for (int x = X1; x <= X2; ++x) {
		for (int y = Y1; y <= Y2; ++y) {
			Mat tile;
			const int ROWS = m_tile_provider->getRowsOfLayer(z);
			const int COLS = m_tile_provider->getColsOfLayer(z);
			if (x >= 0 && x < COLS  && y >= 0 && y < ROWS){
				tile = m_tile_provider->getTile(x, y, z);
			}
			else {
				tile = Mat::zeros(LEN, LEN, CV_8UC3);
				tile.setTo(default_color);
			}
			Rect tile_rect(x * LEN, y * LEN, LEN, LEN);
			copyMatToMat(tile, tile_rect, result, rect);
		}
	}

	return result;
}
void SceneFrameProvider::copyMatToMat(Mat& src_mat, Rect& src_rect, Mat& dst_mat, Rect& dst_rect)
{
	Rect src(0, 0, src_mat.cols, src_mat.rows);
	Rect dst(src_rect.x - dst_rect.x, src_rect.y - dst_rect.y, src_mat.cols, src_mat.rows);

	if (dst.x + dst.width > dst_mat.cols)
	{
		int shift = dst.x + dst.width - dst_mat.cols;
		src.width -= shift;
		dst.width -= shift;
	}
	if (dst.y + dst.height > dst_mat.rows)
	{
		src.height -= dst.y + dst.height - dst_mat.rows;
		dst.height = src.height;
	}
	if (dst.x < 0)
	{
		int shift = -dst.x;
		src.x += shift;
		src.width -= shift;
		dst.x += shift;
		dst.width -= shift;
	}
	if (dst.y < 0)
	{
		int shift = -dst.y;
		src.y += shift;
		src.height -= shift;
		dst.y += shift;
		dst.height -= shift;
	}
	src_mat(src).copyTo(dst_mat(dst));
}
void SceneFrameProvider::incXY(double z, int dx, int dy, double& x, double& y){
	int layer_id = static_cast<int>(z);
	int layer_num = m_tile_provider->getNumLayers();
	layer_id = max(layer_id, 0);
	layer_id = min(layer_id, layer_num - 1);

	x += static_cast<double>(dx) / m_tile_provider->getPixelColsOfLayer(layer_id);
	y += static_cast<double>(dy) / m_tile_provider->getPixelRowsOfLayer(layer_id);

	x = max(x, 0.0);
	x = min(x, 1.0);

	y = max(y, 0.0);
	y = min(y, 1.0);
}