#include "Data.hpp"

#include <cassert>
#include <iostream>
using namespace std;
using namespace cv;

#include "DirDealer.h"

const string TILE_NAME_SUFFIX = ".jpg";

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
void LayerData::loadFromDisk(std::string path, std::string dir) {
	this->dir = dir;
	string path_layer = path + dir + "/";
	DirDealer dir_dealer;
	rows = 0;
	cols = 0;
	tile_name_list.clear();
	if (dir == "0" || dir == "1" || dir == "2") {
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
void SceneData::loadFromDisk(std::string path) {
	const int n_layers = 5;
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
		layer[i].loadFromDisk(path, list_layer[i]);
	}
}
