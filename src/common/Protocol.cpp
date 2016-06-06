#include "Protocol.hpp"

#include <cassert>
#include <string.h>
#include <iostream>
using namespace std;

int Protocol::CMD_LEN = 10;
int Protocol::LEN_LEN = 10;

void Protocol::decodeHead(const std::vector<unsigned char> &buf,
                          std::string &cmd, int &data_len) {
  assert((int)buf.size() == CMD_LEN + LEN_LEN);

  vector<char> cmd_buf(CMD_LEN + 1, 0);
  for (int i = 0; i < CMD_LEN; ++i) {
    cmd_buf[i] = buf[i];
  }
  cmd = string(&cmd_buf[0]);

  memcpy(&data_len, &buf[CMD_LEN], sizeof(int));
}
void Protocol::encodeLength(std::vector<unsigned char> &buf, int length) {
  int data_len = sizeof(int);
  buf.resize(CMD_LEN + CMD_LEN + data_len);

  encodeString(&buf[0], "length");
  encodeInt(&buf[CMD_LEN], data_len);
  encodeInt(&buf[CMD_LEN + LEN_LEN], length);
}
void Protocol::decodeLength(const std::vector<unsigned char> &data, int &length) {
  decodeInt(data, length);
}
void Protocol::encodeSize(std::vector<unsigned char> &buf, int width,
                          int height) {
  vector<int> raw(2);
  raw[0] = width;
  raw[1] = height;

  int data_len = sizeof(int) * raw.size();
  buf.resize(CMD_LEN + CMD_LEN + data_len);

  encodeString(&buf[0], "size");
  encodeInt(&buf[CMD_LEN], data_len);
  encodeVectorInt(&buf[CMD_LEN + LEN_LEN], raw);
}
void Protocol::decodeSize(const std::vector<unsigned char> &data, int &width,
                          int &height) {
  vector<int> raw(2);
  decodeVectorInt(data, raw);
  width = raw[0];
  height = raw[1];
}
void Protocol::encodePoint3d(std::vector<unsigned char> &buf, double x,
                             double y, double z) {
  vector<double> raw(3);
  raw[0] = x;
  raw[1] = y;
  raw[2] = z;

  int data_len = sizeof(double) * raw.size();
  buf.resize(CMD_LEN + CMD_LEN + data_len);

  encodeString(&buf[0], "point3d");
  encodeInt(&buf[CMD_LEN], data_len);
  encodeVectorDouble(&buf[CMD_LEN + LEN_LEN], raw);
}
void Protocol::decodePoint3d(const std::vector<unsigned char> &data, double &x,
                             double &y, double &z) {
  vector<double> raw(3);
  decodeVectorDouble(data, raw);
  x = raw[0];
  y = raw[1];
  z = raw[2];
}
void Protocol::encodeData(std::vector<unsigned char> &buf, std::string cmd,
                          const std::vector<unsigned char> &data) {
  buf.resize(CMD_LEN + CMD_LEN);
  encodeString(&buf[0], cmd);
  encodeInt(&buf[CMD_LEN], data.size());
  buf.insert(buf.end(), data.begin(), data.end());
}
// private
// -------------------------------------------------------------------------------------
void Protocol::encodeString(unsigned char *buf, std::string str) {
  assert((int)str.length() < CMD_LEN);
  memcpy(buf, str.c_str(), str.length());
  buf[str.length()] = 0;
}
void Protocol::encodeInt(unsigned char *buf, int raw) {
  memcpy(buf, &raw, sizeof(int));
}
void Protocol::decodeInt(const std::vector<unsigned char> &buf, int& raw) {
  assert(buf.size() == sizeof(int));
  memcpy(&raw, buf.data(), buf.size());
}
void Protocol::encodeVectorInt(unsigned char *buf,
                               const std::vector<int> &raw) {
  memcpy(buf, raw.data(), sizeof(int) * raw.size());
}
void Protocol::decodeVectorInt(const std::vector<unsigned char> &buf,
                               std::vector<int> &raw) {
  assert(buf.size() % sizeof(int) == 0);
  raw.resize(buf.size() / sizeof(int));
  memcpy(raw.data(), buf.data(), buf.size());
}
void Protocol::encodeVectorDouble(unsigned char *buf,
                                  const std::vector<double> &raw) {
  memcpy(buf, raw.data(), sizeof(double) * raw.size());
}
void Protocol::decodeVectorDouble(const std::vector<unsigned char> &buf,
                                  std::vector<double> &raw) {
  assert(buf.size() % sizeof(double) == 0);
  raw.resize(buf.size() / sizeof(double));
  memcpy(raw.data(), buf.data(), buf.size());
}