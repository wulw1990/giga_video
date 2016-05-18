#include "TileProvider.hpp"

#include <cassert>
#include <iostream>
#include <sys/time.h>
#include <cstdlib>
using namespace std;
using namespace cv;

#include "DirDealer.h"

const string TILE_NAME_SUFFIX = ".jpg";
const Scalar default_color(127, 127, 127);

void LayerData::save(std::ofstream& fout) {
	fout << dir << "\t" << rows << "\t" << cols << endl;
	assert( static_cast<int>(tile_name_list.size()) == rows * cols );
	for (size_t i = 0; i < tile_name_list.size(); ++i) {
		fout << tile_name_list[i] << endl;
	}
}
void LayerData::load(std::ifstream& fin) {
	fin >> dir >> rows >> cols;
	tile_name_list.resize(rows * cols);
	for (int i = 0; i < rows * cols; ++i) {
		fin >> tile_name_list[i];
	}
}
static bool isHeadLayer(string dir, int head_layers){
	for(int i=0; i<head_layers; ++i){
		if(dir==to_string(i)) return true;
	}
	return false;
}
void LayerData::loadFromDisk(std::string path, std::string dir, int head_layers) {
	this->dir = dir;
	string path_layer = path + dir + "/";
	DirDealer dir_dealer;
	rows = 0;
	cols = 0;
	tile_name_list.clear();
	if (isHeadLayer(dir, head_layers)) {
		vector<string> list = dir_dealer.getFileList(path_layer);
		for (size_t i = 0; i < list.size(); ++i) {
			// cout << tile_name_list[i] << endl;
			string name = list[i];
			std::size_t pos1 = name.find("_");
			std::size_t pos2 = name.find(".");
			if (pos1 == std::string::npos || pos2 == std::string::npos) {
				cerr << "name error" << endl;
				exit(-1);
			}
			// cout << pos1 << "\t" << pos2 << endl;
			int r = stoi( name.substr(0, pos1) );
			int c = stoi( name.substr(pos1 + 1, pos2 - pos1) );
			// cout << r << " " << c << endl;
			rows = max(rows, r);
			cols = max(cols, c);
		}
		rows++;
		cols++;
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				string tile_name = dir + "/" + to_string(r) + "_" + to_string(c) + TILE_NAME_SUFFIX;
				tile_name_list.push_back(tile_name);
			}
		}
	} else {
		vector<string> row_name_list = dir_dealer.getFileList(path_layer);
		assert (!row_name_list.empty());
		vector<string> col_name_list = dir_dealer.getFileList(path_layer + row_name_list[0]);
		rows = static_cast<int>(row_name_list.size());
		cols = static_cast<int>(col_name_list.size());
		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				string tile_name = dir + "/" + to_string(r) + "/" + to_string(c) + TILE_NAME_SUFFIX;
				tile_name_list.push_back(tile_name);
			}
		}
	}
}
void SceneData::load(std::string file) {
	ifstream fin(file.c_str());
	assert(fin.is_open());
	int n_layers = -1;
	fin >> tile_len;
	fin >> n_layers;
	layer.resize(n_layers);
	for (size_t i = 0; i < layer.size(); ++i) {
		layer[i].load(fin);
	}
}
void SceneData::save(std::string file) {
	ofstream fout(file.c_str());
	assert(fout.is_open());
	fout << tile_len << endl;
	fout << layer.size() << endl;
	for (size_t i = 0; i < layer.size(); ++i) {
		cout << "saving layer " << i << endl;
		layer[i].save(fout);
	}
}
void SceneData::loadFromDisk(std::string path, int n_layers, int head_layers) {
	// const int n_layers = 7;
	tile_len = 512;
	DirDealer dir_dealer;
	vector<string> list_layer(n_layers);
	for (int i = 0; i < n_layers; ++i) {
		list_layer[i] = to_string(i);
		if (!dir_dealer.existFileOrDir(path + list_layer[i])) {
			cerr << "layer error: " << list_layer[i] << endl;
			exit(-1);
		}
	}
	layer.resize(n_layers);
	for (size_t i = 0; i < list_layer.size(); ++i) {
		cout << "loading layer " << i << endl;
		layer[i].loadFromDisk(path, list_layer[i], head_layers);
	}
}
TileProvider::TileProvider(std::string path, std::string info_file) {
	this->path = path;
	m_scene_data = make_shared<SceneData>();
	m_scene_data->load(info_file);

	m_pixel_rows.resize(m_scene_data->getNumLayers());
	m_pixel_cols.resize(m_scene_data->getNumLayers());

	int top_id = m_scene_data->getNumLayers() - 1;
	int top_rows = m_scene_data->getRowsOfLayer(top_id) * m_scene_data->getTileLen();
	int top_cols = m_scene_data->getColsOfLayer(top_id) * m_scene_data->getTileLen();

	m_pixel_rows[top_id] = top_rows;
	m_pixel_cols[top_id] = top_cols;

	for (int i = top_id - 1; i >= 0; --i ) {
		m_pixel_rows[i] = m_pixel_rows[i + 1] / 2;
		m_pixel_cols[i] = m_pixel_cols[i + 1] / 2;
		// cout << i << "\trows: " << m_pixel_rows[i] << "\tcols: " << m_pixel_cols[i] << endl;
	}
}
int TileProvider::getTileLen() {
	return m_scene_data->getTileLen();
}
int TileProvider::getNumLayers() {
	return m_scene_data->getNumLayers();
}
int TileProvider::getRowsOfLayer(int layer_id) {
	return m_scene_data->getRowsOfLayer(layer_id);
}
int TileProvider::getColsOfLayer(int layer_id) {
	return m_scene_data->getColsOfLayer(layer_id);
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
	string name = m_scene_data->getTileName(x, y, z);
	if (m_cache.find(name) == m_cache.end()) {
		m_cache[name] = make_pair(imread(path + name), getCurrentTimeFromStart());
		if (is_cache) { *is_cache = 0; }
	} else {
		if (is_cache) { *is_cache = 1; }
	}
	cv::Mat tile = m_cache[name].first;
	m_cache[name].second = getCurrentTimeFromStart();

	assert(!tile.empty());
	if (tile.rows < getTileLen() || tile.cols < getTileLen()) {
		// cout << "hh" << endl;
		Mat tmp(getTileLen(), getTileLen(), tile.type());
		tmp.setTo(default_color);
		Rect rect(0, 0, tile.cols, tile.rows);
		tile.copyTo(tmp(rect));
		tile = tmp;
	}
	// cout << rand() << endl;
	resizeCache();

	return tile;
}
long long TileProvider::getCurrentTimeFromStart() {
	static bool first = true;
	static struct timeval t1;
	if (first) {
		gettimeofday(&t1, NULL);
		first = false;
	}
	struct timeval t2;
	gettimeofday(&t2, NULL);
	return (t2.tv_sec - t1.tv_sec) * 1e6 + (t2.tv_usec - t1.tv_usec);
}
void TileProvider::resizeCache() {
	const int MAX_SIZE = 1024;
	if (m_cache.size() > MAX_SIZE) {
		vector<long long> time_vec;
		for (auto iter = m_cache.begin(); iter != m_cache.end(); ++iter) {
			time_vec.push_back(iter->second.second);
		}
		sort(time_vec.begin(), time_vec.end());

		long long thresh = time_vec[time_vec.size() - MAX_SIZE / 2];

		for (auto iter = m_cache.begin(); iter != m_cache.end(); ) {
			if (iter->second.second < thresh) {
				iter = m_cache.erase(iter);
			} else {
				++iter;
			}
		}
	}
}