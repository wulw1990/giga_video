#ifndef DATA_PROVIDER_HPP_
#define DATA_PROVIDER_HPP_

#include <memory>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <map>
class SceneData;

class TileProvider {
	public:
		TileProvider(std::string path, std::string info_file);
		int getTileLen();
		int getNumLayers();
		int getRowsOfLayer(int layer_id);
		int getColsOfLayer(int layer_id);
		int getPixelRowsOfLayer(int layer_id);
		int getPixelColsOfLayer(int layer_id);

		cv::Mat getTile(int x, int y, int z);//cached

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
		float getCurrentTime();
		std::map<std::string, std::pair<cv::Mat, float> > m_cache;
};
class SceneFrameProvider {
	public:
		SceneFrameProvider(std::string path, std::string info_file);
		cv::Mat getFrame(int w, int h, double x, double y, double z);
		cv::Mat getFrame(int w, int h, int x, int y, int z);
		void incXY(double z, int dx, int dy, double& x, double& y);
		double getMaxZ() { return 5.0; }
		double getMinZ() { return -3.5; }
		int getLayerWidth(int layer_id);
		int getLayerHeight(int layer_id);

	private:
		std::shared_ptr<TileProvider> m_tile_provider;

		void copyMatToMat(cv::Mat& src_mat, cv::Rect& src_rect, cv::Mat& dst_mat, cv::Rect& dst_rect);
};



#endif