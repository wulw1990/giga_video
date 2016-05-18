#ifndef DATA_PROVIDER_HPP_
#define DATA_PROVIDER_HPP_

#include <memory>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <map>

class SceneData;
class MultiVideoData;
class VideoProvider;
class TileProvider;

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
		std::shared_ptr<VideoProvider> m_video_data;

		int frame_width;
		int frame_height;
		double x;
		double y;
		double z;

		void copyMatToMat(cv::Mat& src_mat, cv::Rect& src_rect, cv::Mat& dst_mat, cv::Rect& dst_rect);
};

#endif