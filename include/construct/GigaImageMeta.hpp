#ifndef GIGA_IMAGE_META_HPP_
#define GIGA_IMAGE_META_HPP_

#include <string>
#include <vector>

class GigaImageMeta {
public:
  void generateMetaFile(std::string path, std::string meta);
  void load(std::string meta);
  int getLayers();
  int getRows(int layer_id);
  int getCols(int layer_id);

private:
  // tile name: layer row col
  std::vector<std::vector<std::vector<std::string>>> m_tile_name;
};

#endif