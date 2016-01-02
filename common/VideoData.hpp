#ifndef VIDEO_DATA_HPP_
#define VIDEO_DATA_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <opencv2/opencv.hpp>

class CameraBase;

class VideoData {
	public:
		VideoData(std::string path);
		void load();
		void save(std::string video_src, cv::Mat H, cv::Rect rect_on_scene);

		cv::Mat getH() {return H.clone();}
		cv::Rect getRectOnScene() {return rect_on_scene;}
		cv::Mat getFrame();

	private:
		std::string path;
		const std::string NAME_VIDEO;
		const std::string NAME_INFO;
		cv::Mat H;
		cv::Rect rect_on_scene;

		cv::VideoCapture capture;
		void openCapture();
};

class MultiVideoData {
	public:
		MultiVideoData(std::string path);
		int getNumVideos() {return static_cast<int>(video_data.size());}
		cv::Rect getRectOnScene(int video_id) {
			assert(video_id >= 0 && video_id < getNumVideos() );
			return video_data[video_id].getRectOnScene();
		}
		cv::Mat getFrame(int video_id) {
			assert(video_id >= 0 && video_id < getNumVideos() );
			return video_data[video_id].getFrame();
		}

	private:
		std::string path;
		const std::string NAME_LIST;
		std::vector<VideoData> video_data;
};

class CameraSetData{
	public:
		CameraSetData(std::string path);
		int getNumCamera();
		bool getRectOnScene(cv::Rect& rect, int index);
		bool getFrame(cv::Mat& frame, int index);

	private:
		std::string m_path;
		std::shared_ptr<CameraBase> m_camera_set;

		std::vector<cv::Mat> m_trans;
		std::vector<cv::Rect> m_rect;
};

#endif