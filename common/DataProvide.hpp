#ifndef DATA_PROVIDER_HPP_
#define DATA_PROVIDER_HPP_

#include <memory>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <map>

class SceneData;
class MultiVideoData;
class CameraSetData;

class TileProvider {
	public:
		TileProvider(std::string path, std::string info_file);
		int getTileLen();
		int getNumLayers();
		int getRowsOfLayer(int layer_id);
		int getColsOfLayer(int layer_id);
		int getPixelRowsOfLayer(int layer_id);
		int getPixelColsOfLayer(int layer_id);

		cv::Mat getTile(int x, int y, int z, int* is_cache = NULL); //cached

	private:
		std::string path;
		std::shared_ptr<SceneData> m_scene_data;
		std::vector<int> m_pixel_rows;
		std::vector<int> m_pixel_cols;

		// class Node
		// {
		// 	public:
		// 		Node(cv::Mat data_) {
		// 			data = data_.clone();
		// 			updateTime();
		// 		}
		// 		void updateTime();
		// 		float getTime() {return t;}
		// 		cv::Mat getData() {return data;}
		// 	private:
		// 		cv::Mat data;
		// 		float t;
		// };
		std::map<std::string, std::pair<cv::Mat, long long> > m_cache;
		long long getCurrentTimeFromStart();
		void resizeCache();
};
class FrameProvider {
	public:
		FrameProvider(std::string path, bool enable_video);
		cv::Mat getFrame(int w, int h, double x, double y, double z);
		cv::Mat getFrame(int w, int h, int x, int y, int z);
		void getFrameWithMask(cv::Mat& frame, cv::Mat& mask, int w, int h, double x, double y, double z);
		void getFrameWithMask(cv::Mat& frame, cv::Mat& mask, int w, int h, int x, int y, int z);
		int getNumLayers();
		int getLayerWidth(int layer_id);
		int getLayerHeight(int layer_id);

	private:
		std::shared_ptr<TileProvider> m_tile_provider;

		bool m_enable_video;
		// std::shared_ptr<MultiVideoData> m_multi_video_data;
		std::shared_ptr<CameraSetData> m_video_data;

		int frame_width;
		int frame_height;
		double x;
		double y;
		double z;

		void copyMatToMat(cv::Mat& src_mat, cv::Rect& src_rect, cv::Mat& dst_mat, cv::Rect& dst_rect);
};

#endif