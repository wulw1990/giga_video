#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>
using namespace std;

#include "ImagePlayer.hpp"

int image(int argc, char** argv);
int video(int argc, char** argv);

int main(int argc, char **argv){
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if(mode == "image") image(argc, argv);
	else if(mode == "video") video(argc, argv);
	else {
		cerr << "main mode error : " << mode << endl;
		return -1;
	}
	return 0;
}
int image(int argc, char** argv){
	cout << "image demo" << endl;
	string window = "giga image player";
	assert(argc>=3);

	ImagePlayer player( argv[1], argv[2]);
	player.play();

	return 0;
}
int video(int argc, char** argv){
	cout << "video demo" << endl;
	return 0;
}