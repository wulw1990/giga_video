#include "TileProvider.hpp"

#include <cassert>
#include <iostream>
#include <sys/time.h>
#include <cstdlib>
using namespace std;
using namespace cv;

#include "DirDealer.h"
#include "GigaImageMeta.hpp"

const string TILE_NAME_SUFFIX = ".jpg";
const Scalar default_color(127, 127, 127);

TileProvider::TileProvider(std::string path, std::string info_file) {
	m_path = path;
	m_meta = make_shared<GigaImageMeta>();
	m_meta->load(path + "info_scene.txt");

	m_pixel_rows.resize(m_meta->getLayers());
	m_pixel_cols.resize(m_meta->getLayers());

	int top_id = m_meta->getLayers() - 1;
	int top_rows = m_meta->getRows(top_id) * m_meta->getTileLen();
	int top_cols = m_meta->getCols(top_id) * m_meta->getTileLen();

	m_pixel_rows[top_id] = top_rows;
	m_pixel_cols[top_id] = top_cols;

	for (int i = top_id - 1; i >= 0; --i ) {
		m_pixel_rows[i] = m_pixel_rows[i + 1] / 2;
		m_pixel_cols[i] = m_pixel_cols[i + 1] / 2;
		// cout << i << "\trows: " << m_pixel_rows[i] << "\tcols: " << m_pixel_cols[i] << endl;
	}
}
int TileProvider::getTileLen() {
	return m_meta->getTileLen();
}
int TileProvider::getNumLayers() {
	return m_meta->getLayers();
}
int TileProvider::getRowsOfLayer(int layer_id) {
	return m_meta->getRows(layer_id);
}
int TileProvider::getColsOfLayer(int layer_id) {
	return m_meta->getCols(layer_id);
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
	static map<string, Scalar> mm;
	Scalar color;
	string p = to_string(x) + "_" + to_string(y) + "_" +to_string(z);
	if(mm.find(p)==mm.end()){
		color = Scalar(rand()%255, rand()%255, rand()%255);
		mm[p] = color;
	}else{
		color = mm[p];
	}
	Mat tile = Mat(getTileLen(), getTileLen(), CV_8UC3);
	tile.setTo(color);
	return tile;
}
