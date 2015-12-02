#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>
using namespace std;

#include "SocketServer.hpp"

int demo(int argc, char** argv);

int main(int argc, char **argv){
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if(mode == "demo") demo(argc, argv);
	else {
		cerr << "main mode error : " << mode << endl;
		return -1;
	}
	return 0;
}
int demo(int argc, char** argv){
	cout << "server demo" << endl;
	init_server();
	return 0;
}