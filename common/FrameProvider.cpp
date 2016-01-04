#include "FrameProvider.hpp"

using namespace std;
using namespace cv;

#include "TileProvider.hpp"
#include "VideoProvider.hpp"


const Scalar default_color(127, 127, 127);

FrameProvider::FrameProvider(std::string path, bool enable_video) {
	m_tile_provider = make_shared<TileProvider>(path, path + "info_scene.txt");

	m_enable_video = enable_video;
	if (m_enable_video) {
		// m_multi_video_data = make_shared<MultiVideoData>(path + "video/");
		m_video_data = make_shared<VideoProvider>( path + "video/");
	}
}
void FrameProvider::getFrameWithMask(cv::Mat& frame, cv::Mat& mask, int w, int h, double x, double y, double z){
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

	// Mat frame, mask;
	getFrameWithMask(frame, mask, sw, sh, pixel_x, pixel_y, layer_id);
	// cout << zoom << endl;
	// cout << sw << "\t" << sh << endl;
	// cout << result.size() << endl;
	resize(frame, frame, Size(w, h));
	resize(mask, mask, Size(w, h));
}
void FrameProvider::getFrameWithMask(cv::Mat& frame, cv::Mat& mask, int w, int h, int x, int y, int z){
	const int LEN = m_tile_provider->getTileLen();

	Rect rect(x, y, w, h);
	frame = Mat(h, w, CV_8UC3);
	frame.setTo(default_color);//default color

	mask = Mat(h, w, CV_8UC1);
	mask.setTo(0);

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
			copyMatToMat(tile, tile_rect, frame, rect);
		}
	}
	// cout << "cache_count: " << cache_count << endl;
	// cout << "not_cache_count: " << not_cache_count << endl;



	if (m_enable_video && z == m_tile_provider->getNumLayers() - 1) {
		cout << "hh " << endl;
		// int n_videos = m_multi_video_data->getNumVideos();
		int n_videos = m_video_data->getNumCamera();
		cout << "n_videos:  " << n_videos << endl;

		for (int i = 0; i < n_videos; ++i) {
			// Rect rect_video_on_scene = m_multi_video_data->getRectOnScene(i);
			Rect rect_video_on_scene;
			assert(m_video_data->getRectOnScene(rect_video_on_scene, i));

			Rect rect_overlap = rect_video_on_scene & Rect(x, y, w, h);
			if (rect_overlap.width > 0 && rect_overlap.height > 0) {
				// cout << "dd " << endl;
				// Mat video_frame = m_multi_video_data->getFrame(i);
				Mat video_frame;
				assert(m_video_data->getFrame(video_frame, i));
				// imshow("video_frame", video_frame);

				Rect rect_on_video = rect_overlap;
				rect_on_video.x -= rect_video_on_scene.x;
				rect_on_video.y -= rect_video_on_scene.y;

				Rect rect_on_win = rect_overlap;
				rect_on_win.x -= x;
				rect_on_win.y -= y;

				//TODO: Optic Aligner
				Mat src = video_frame(rect_on_video);
				Mat dst = frame(rect_on_win);
				Mat dst2 = mask(rect_on_win);
				for (int r = 0; r < src.rows; ++r) {
					for (int c = 0; c < src.cols; ++c) {
						if (src.at<Vec3b>(r, c) != Vec3b(0, 0, 0)) {
							dst.at<Vec3b>(r, c) = src.at<Vec3b>(r, c);
							dst2.at<unsigned char>(r, c) = 255;
						}
					}
				}
			}
		}
	} else if (m_enable_video) {
		// int n_videos = m_multi_video_data->getNumVideos();
		int n_videos = m_video_data->getNumCamera();
		for (int i = 0; i < n_videos; ++i) {
			Rect rect_video_on_scene;
			assert(m_video_data->getRectOnScene(rect_video_on_scene, i));
			// Rect rect_video_on_scene = m_multi_video_data->getRectOnScene(i);
			double scale = pow(2, m_tile_provider->getNumLayers() - 1 - z);
			rect_video_on_scene.x /= scale;
			rect_video_on_scene.y /= scale;
			rect_video_on_scene.width /= scale;
			rect_video_on_scene.height /= scale;
			Rect rect_overlap = rect_video_on_scene & Rect(x, y, w, h);
			if (rect_overlap.width > 0 && rect_overlap.height > 0) {
				rect_overlap.x -= x;
				rect_overlap.y -= y;
				rectangle( frame, rect_overlap, Scalar(255, 0, 0), 2);
				rectangle( mask, rect_overlap, Scalar(255), 2);
			}
		}
	}
}
cv::Mat FrameProvider::getFrame(int w, int h, double x, double y, double z) {
	Mat frame, mask;
	getFrameWithMask(frame, mask, w, h, x, y, z);
	return frame;
}
cv::Mat FrameProvider::getFrame(int w, int h, int x, int y, int z) {
	Mat frame, mask;
	getFrameWithMask(frame, mask, w, h, x, y, z);
	return frame;
}
void FrameProvider::copyMatToMat(Mat & src_mat, Rect & src_rect, Mat & dst_mat, Rect & dst_rect)
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