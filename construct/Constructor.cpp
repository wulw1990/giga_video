#include "Constructor.hpp"

#include <cassert>
#include <map>
#include <iostream>
using namespace std;

#include "Data.hpp"

Constructor::Constructor() {
}
void Constructor::writeInfoForAutopan(std::string path, int n_layers, std::string name) {
	cout << "constructFromAutopan" << endl;
	
	SceneData scene_data;
	scene_data.loadFromDisk(path);
	scene_data.save(path + name);
	
	cout << "verify..." << endl;
	SceneData scene_data_ver;
	scene_data_ver.load(path + name);
	scene_data_ver.save(path + name);
}
