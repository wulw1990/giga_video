#include "Protocol.hpp"

#include <cassert>
#include <string.h>
#include <iostream>
using namespace std;

Protocol::Protocol(): CMD_LEN(10), LEN_LEN(10) {
}
void Protocol::encode(
    std::vector<unsigned char>& buf,
    std::string cmd, const std::vector<unsigned char>& data ) {
	assert(static_cast<int>(cmd.length()) <= CMD_LEN);
	buf.resize(CMD_LEN + LEN_LEN + data.size());
	memset(&buf[0], 0, buf.size());
	memcpy(&buf[0], &cmd[0], cmd.length());

	vector<unsigned char> len_buf;
	int2Vector(data.size(), len_buf);
	memcpy(&buf[CMD_LEN], &len_buf[0], len_buf.size());

	memcpy(&buf[CMD_LEN + LEN_LEN], &data[0], data.size());
}
void Protocol::decodeHead(
    const std::vector<unsigned char>& buf,
    std::string& cmd, int& data_len ) {
	assert((int)buf.size()==CMD_LEN+LEN_LEN);

	vector<char> cmd_buf(CMD_LEN + 1, 0);
	for (int i = 0; i < CMD_LEN; ++i) {
		cmd_buf[i] = buf[i];
	}
	cmd = string(&cmd_buf[0]);

	vector<unsigned char> len_buf(LEN_LEN, 0);
	memcpy(&len_buf[0], &buf[CMD_LEN], LEN_LEN);

	vector2Int(data_len, len_buf);
}
void Protocol::int2Vector(int value, std::vector<unsigned char>& vec) {
	vec.resize(LEN_LEN);
	for (int i = LEN_LEN - 1; i >= 0 && value; --i) {
		vec[i] = value % 10;
		value /= 10;
	}
}
void Protocol::vector2Int(int& value, const std::vector<unsigned char>& vec) {
	assert(static_cast<int>(vec.size()) == LEN_LEN);
	value = 0;
	for (size_t i = 0; i < vec.size(); ++i) {
		value = value * 10 + vec[i];
	}
}