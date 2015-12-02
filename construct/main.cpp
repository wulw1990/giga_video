#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace std;

#include "Constructor.hpp"

int construct_from_autopan(int argc, char** argv);

int main(int argc, char **argv){
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if(mode == "construct_from_autopan") construct_from_autopan(argc, argv);
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