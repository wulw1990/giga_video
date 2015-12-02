#ifndef DATA_PROVIDER_HPP_
#define DATA_PROVIDER_HPP_

#include <memory>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
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
		cv::Mat getTile(int x, int y, int z);

	private:
		std::string path;
		std::shared_ptr<SceneData> m_scene_data;
		std::vector<int> m_pixel_rows;
		std::vector<int> m_pixel_cols;
};
class SceneFrameProvider {
public:
	SceneFrameProvider(std::string path, std::string info_file, int w, int h);
	cv::Mat getFrame(double x, double y, double z);
	void incXY(double z, int dx, int dy, double& x, double& y);
	double getMaxZ(){	return 4.0; }
	double getMinZ(){	return 0.0; }

	private:
		std::shared_ptr<TileProvider> m_tile_provider;
		int m_frame_width;
		int m_frame_height;

	cv::Mat getFrame(int w, int h, int x, int y, int z);
	void copyMatToMat(cv::Mat& src_mat, cv::Rect& src_rect, cv::Mat& dst_mat, cv::Rect& dst_rect);
};



#endif