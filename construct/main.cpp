#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace std;

#include "Constructor.hpp"
#include "DirDealer.h"

int construct_from_autopan(int argc, char** argv);
int construct_video(int argc, char** argv);
int cut_video(int argc, char** argv);

int main(int argc, char **argv){
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if(mode == "construct_from_autopan") construct_from_autopan(argc, argv);
	else if(mode == "construct_video") construct_video(argc, argv);
	else if(mode == "cut_video") cut_video(argc, argv);
	else {
		cerr << "main mode error : " << mode << endl;
		return -1;
	}
	return 0;
}
int construct_from_autopan(int argc, char** argv){
	cout << "construct_from_autopan demo" << endl;
	assert(argc>=2);
	Constructor constructor;
	constructor.writeInfoForAutopan(argv[1]);
	return 0;
}
int construct_video(int argc, char** argv){
	cout << "construct_video demo" << endl;
	assert(argc>=3);
	string raw_video_name(argv[1]);
	string path_output(argv[2]);

	DirDealer dir_dealer;
	dir_dealer.mkdir_p(path_output);

	Constructor constructor;
	constructor.constructVideo(raw_video_name, path_output);
	return 0;
}
int cut_video(int argc, char** argv){
	cout << "cut_video demo" << endl;
	assert(argc>=3);
	string name_src(argv[1]);
	string name_dst(argv[2]);

	Constructor constructor;
	constructor.cutVideo(name_src, name_dst);
	return 0;
}