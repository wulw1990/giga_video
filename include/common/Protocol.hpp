#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include <vector>
#include <string>

class Protocol {
public:
  static void decodeHead(const std::vector<unsigned char> &buf,
                         std::string &cmd, int &data_len);

  static void encodeLength(std::vector<unsigned char> &buf, int length);
  static void decodeLength(const std::vector<unsigned char> &data, int &length);
  static void encodeSize(std::vector<unsigned char> &buf, int width,
                         int height);
  static void decodeSize(const std::vector<unsigned char> &data, int &width,
                         int &height);

  static void encodePoint3d(std::vector<unsigned char> &buf, double x, double y,
                            double z);
  static void decodePoint3d(const std::vector<unsigned char> &data, double &x,
                            double &y, double &z);

  static void encodeData(std::vector<unsigned char> &buf, std::string cmd,
                         const std::vector<unsigned char> &data);

  static int getHeadLen() { return CMD_LEN + LEN_LEN; }

private:
  static int CMD_LEN;
  static int LEN_LEN;

  static void encodeString(unsigned char *buf, std::string str);
  static void encodeInt(unsigned char *buf, int raw);
  static void decodeInt(const std::vector<unsigned char> &buf, int& raw);
  static void encodeVectorInt(unsigned char *buf, const std::vector<int> &raw);
  static void decodeVectorInt(const std::vector<unsigned char> &buf,
                              std::vector<int> &raw);
  static void encodeVectorDouble(unsigned char *buf,
                                 const std::vector<double> &raw);
  static void decodeVectorDouble(const std::vector<unsigned char> &buf,
                                 std::vector<double> &raw);
};

#endif