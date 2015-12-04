#include "Protocol.hpp"

#include <cassert>
#include <string.h>
#include <iostream>
using namespace std;

Protocol::Protocol(): CMD_LEN(4), LEN_LEN(sizeof(int)), DATA_LEN(2048) {
	assert(LEN_LEN==4);
}
void Protocol::encode(
    std::vector<unsigned char>& buf,
    std::string cmd, const std::vector<unsigned char>& data ) {
	assert(static_cast<int>(cmd.length()) <= CMD_LEN);
	assert(static_cast<int>(data.size()) <= DATA_LEN);

	buf.resize(CMD_LEN + LEN_LEN + DATA_LEN);
	memset(&buf[0], 0, buf.size());
	memcpy(&buf[0], &cmd[0], cmd.length());
	for(int i=0; i<10; ++i){
		cout << (int)buf[i] << " ";
	}
	cout << endl;

	cout << "LEN_LEN: " << LEN_LEN << endl;
	int length = static_cast<int>(data.size());
	memcpy(&buf[CMD_LEN], &length, LEN_LEN);

}
void Protocol::decode(
    const std::vector<unsigned char>& buf,
    std::string& cmd, std::vector<unsigned char>& data ) {

}