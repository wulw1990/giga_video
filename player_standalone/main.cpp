#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>
using namespace std;

#include "Player.hpp"
#include "DirDealer.h"

int main(int argc, char **argv){
	assert(argc>2);
	string path(argv[1]);
	string output_video(argv[2]);
	cout << "path: " << path << endl;
	cout << "output_video: " << output_video << endl;
	DirDealer dir_dealer; 
	vector<string> list = dir_dealer.getFileList(path);
	int n_frames = (int)list.size();
	cout << "n_frames: " << n_frames << endl;

	Player player(path, n_frames, output_video);
	player.play();

	return 0;
}
