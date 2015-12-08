#include "DataProvide.hpp"

#include <sys/time.h>
#include <cstdlib>
using namespace std;
using namespace cv;

#include "Data.hpp"


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
cv::Mat TileProvider::getTile(int x, int y, int z, int* is_cache) {
	string name = m_scene_data->getTileName(x, y, z);
	// cout << "name: " << name << endl;
	if (m_cache.find(name) == m_cache.end()) {
		m_cache[name] = make_pair(imread(path + name), getCurrentTimeFromStart());
		if (is_cache) { *is_cache = 0; }
	} else {
		// cout << "cached" << endl;
		if (is_cache) { *is_cache = 1; }
	}
	cv::Mat tile = m_cache[name].first;
	m_cache[name].second = getCurrentTimeFromStart();
	// cout << m_scene_data->getTileName(x, y, z) << endl;

	assert(!tile.empty());
	if (tile.rows < getTileLen() || tile.cols < getTileLen()) {
		Mat tmp(getTileLen(), getTileLen(), tile.type());
		tmp.setTo(default_color);
		Rect rect(0, 0, tile.cols, tile.rows);
		tile.copyTo(tmp(rect));
		tile = tmp;
	}
	resizeCache();

	return tile;
}
long long TileProvider::getCurrentTimeFromStart() {
	static bool first = true;
	static struct timeval t1;
	if (first) {
		gettimeofday(&t1, NULL);
		first = false;
	}
	struct timeval t2;
	gettimeofday(&t2, NULL);
	// cout << "t_.tv_sec: " << t_.tv_sec << endl;
	// cout << "t_.tv_usec: " << t_.tv_usec << endl;
	return (t2.tv_sec - t1.tv_sec) * 1e6 + (t2.tv_usec - t1.tv_usec);
}
void TileProvider::resizeCache() {
	const int MAX_SIZE = 1024;
	if (m_cache.size() > MAX_SIZE) {
		// cout << m_cache.size() << endl;
		vector<long long> time_vec;
		for (auto iter = m_cache.begin(); iter != m_cache.end(); ++iter) {
			time_vec.push_back(iter->second.second);
			// cout << iter->second.second << " ";
		}
		// cout << endl;
		sort(time_vec.begin(), time_vec.end());
		// cout << time_vec.size() << endl;

		long long thresh = time_vec[time_vec.size() - MAX_SIZE / 2];
		// cout << "thresh: " << thresh << endl;

		// cout << m_cache.size() << endl;
		for (auto iter = m_cache.begin(); iter != m_cache.end(); ) {
			if (iter->second.second < thresh) {
				iter = m_cache.erase(iter);
			} else {
				++iter;
			}
		}
		// cout << m_cache.size() << endl;

		// for (auto iter = m_cache.begin(); iter != m_cache.end(); ++iter) {
		// cout << iter->second.second << " ";
		// }
		// cout << endl;
	}
}




FrameProvider::FrameProvider(std::string path, bool enable_video) {
	m_tile_provider = make_shared<TileProvider>(path, path+"info_scene.txt");

	m_enable_video = enable_video;
	if (m_enable_video) {
		m_multi_video_data = make_shared<MultiVideoData>(path + "video/");
	}
}
cv::Mat FrameProvider::getFrame(int w, int h, double x, double y, double z) {
	int layer_id = static_cast<int>(z + 1);
	layer_id = max(layer_id, 0);
	layer_id = min(layer_id, m_tile_provider->getNumLayers() - 1);

	int pixel_x = static_cast<int>(x * m_tile_provider->getPixelColsOfLayer(layer_id));
	int pixel_y = static_cast<int>(y * m_tile_provider->getPixelRowsOfLayer(layer_id));

	double zoom = pow(2.0, (double)layer_id - z);
	int sw = zoom * w;
	int sh = zoom * h;

	pixel_x -= sw / 2;
	pixel_y -= sh / 2;

	// cout << pixel_x << "\t" << pixel_y << endl;

	Mat result = getFrame(sw, sh, pixel_x, pixel_y, layer_id);
	// cout << zoom << endl;
	// cout << sw << "\t" << sh << endl;
	// cout << result.size() << endl;
	resize(result, result, Size(w, h));
	return result;
}
cv::Mat FrameProvider::getFrame(int w, int h, int x, int y, int z) {
	const int LEN = m_tile_provider->getTileLen();

	Rect rect(x, y, w, h);
	Mat result(h, w, CV_8UC3);
	result.setTo(default_color);//default color

	int X1 = x / LEN;
	int Y1 = y / LEN;
	int X2 = (x + w - 1) / LEN;
	int Y2 = (y + h - 1) / LEN;

	int cache_count = 0;
	int not_cache_count = 0;
	Mat tile;
	for (int x = X1; x <= X2; ++x) {
		for (int y = Y1; y <= Y2; ++y) {
			Mat tile;
			const int ROWS = m_tile_provider->getRowsOfLayer(z);
			const int COLS = m_tile_provider->getColsOfLayer(z);
			if (x >= 0 && x < COLS  && y >= 0 && y < ROWS) {
				int is_cache;
				tile = m_tile_provider->getTile(x, y, z, &is_cache);
				cache_count += is_cache;
				not_cache_count += !is_cache;
			}
			else {
				tile = Mat::zeros(LEN, LEN, CV_8UC3);
				tile.setTo(default_color);
			}
			Rect tile_rect(x * LEN, y * LEN, LEN, LEN);
			copyMatToMat(tile, tile_rect, result, rect);
		}
	}
	// cout << "cache_count: " << cache_count << endl;
	// cout << "not_cache_count: " << not_cache_count << endl;



	if(m_enable_video && z==m_tile_provider->getNumLayers()-1){
		// cout << "hh " << endl;
		int n_videos = m_multi_video_data->getNumVideos();
		for(int i=0; i<n_videos; ++i){
			Rect rect_video_on_scene = m_multi_video_data->getRectOnScene(i);
			Rect rect_overlap = rect_video_on_scene & Rect(x, y, w, h);
			if(rect_overlap.width>0 && rect_overlap.height>0){
				// cout << "dd " << endl;
				Mat video_frame = m_multi_video_data->getFrame(i);
				Rect rect_on_video = rect_overlap;
				rect_on_video.x -= rect_video_on_scene.x;
				rect_on_video.y -= rect_video_on_scene.y;

				Rect rect_on_win = rect_overlap;
				rect_on_win.x -= x;
				rect_on_win.y -= y;

				//TODO: Optic Aligner
				Mat src = video_frame(rect_on_video);
				Mat dst = result(rect_on_win);
				for(int r=0; r<src.rows; ++r){
					for(int c=0; c<src.cols; ++c){
						if(src.at<Vec3b>(r,c)!=Vec3b(0,0,0)){
							dst.at<Vec3b>(r,c) = src.at<Vec3b>(r,c);
						}
					}
				}
			}
		}
	}



	return result;
}
void FrameProvider::copyMatToMat(Mat& src_mat, Rect& src_rect, Mat& dst_mat, Rect& dst_rect)
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
int FrameProvider::getNumLayers() {
	return m_tile_provider->getNumLayers();
}
int FrameProvider::getLayerWidth(int layer_id) {
	return m_tile_provider->getPixelColsOfLayer(layer_id);
}
int FrameProvider::getLayerHeight(int layer_id) {
	return m_tile_provider->getPixelRowsOfLayer(layer_id);
}