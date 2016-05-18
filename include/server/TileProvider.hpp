#ifndef TILE_PROVIDER_HPP_
#define TILE_PROVIDER_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <opencv2/opencv.hpp>

class GigaImageMeta;

class TileProvider {
public:
  TileProvider(std::string path, std::string info_file);
  int getTileLen();
  int getNumLayers();
  int getRowsOfLayer(int layer_id);
  int getColsOfLayer(int layer_id);
  int getPixelRowsOfLayer(int layer_id);
  int getPixelColsOfLayer(int layer_id);

  void getTile(const std::vector<int> &x, const std::vector<int> &y,
               const std::vector<int> &z, std::vector<cv::Mat> &tile);

private:
  std::string m_path;
  std::vector<int> m_pixel_rows;
  std::vector<int> m_pixel_cols;

  std::shared_ptr<GigaImageMeta> m_meta;
};

#endif
