#ifndef DATA_HPP_
#define DATA_HPP_

#include <vector>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>

class LayerData {
	public:
		void load(std::ifstream& fin);
		void save(std::ofstream& fout);
		void loadFromDisk(std::string path, std::string dir);//slow

		int getRows() {return rows;}
		int getCols() {return cols;}
		std::string getDir() {return dir;}
		std::string getTileName(int r, int c) {return tile_name_list[r * cols + c];}

	private:
		int rows;
		int cols;
		std::string dir;
		std::vector<std::string> tile_name_list;
};
class SceneData
{
	public:
		void load(std::string file);
		void save(std::string file);
		void loadFromDisk(std::string path);//slow

		int getTileLen() {
			return tile_len;
		}
		int getNumLayers() {
			return static_cast<int>(layer.size());
		}
		int getRowsOfLayer(int layer_id) {
			assert(layer_id >= 0 && layer_id < getNumLayers());
			return layer[layer_id].getRows();
		}
		int getColsOfLayer(int layer_id) {
			assert(layer_id >= 0 && layer_id < getNumLayers());
			return layer[layer_id].getCols();
		}
		std::string getTileName(int x, int y, int z) {
			assert(z >= 0 && z < getNumLayers());
			assert(x >= 0 && x < getColsOfLayer(z));
			assert(y >= 0 && y < getColsOfLayer(z));
			return layer[z].getTileName(y, x);
		}
	private:
		std::vector<LayerData> layer;
		int tile_len;
};


#endif
