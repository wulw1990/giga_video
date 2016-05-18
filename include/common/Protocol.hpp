#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include <vector>
#include <string>

class Protocol {
public:
  static void encode(std::vector<unsigned char> &buf, std::string cmd,
                     const std::vector<unsigned char> &data);

  static void encodeHead(std::vector<unsigned char> &head, std::string cmd,
                         int data_len);

  static void decodeHead(const std::vector<unsigned char> &buf,
                         std::string &cmd, int &data_len);

  static void encodeXYZ(std::vector<unsigned char> &buf, int x, int y, int z);

  static void decodeDataXYZ(const std::vector<unsigned char> &buf, int &x,
                            int &y, int &z);

  static void encodeFloatVector(const std::vector<float> &vec,
                                std::vector<unsigned char> &buf);

  static void decodeFloatVector(const std::vector<unsigned char> &buf,
                                std::vector<float> &fvec);

  static int getHeadLen() { return CMD_LEN + LEN_LEN; }

private:
  static int CMD_LEN;
  static int LEN_LEN;

  static void int2Vector(int value, std::vector<unsigned char> &vec);
  static void vector2Int(int &value, const std::vector<unsigned char> &vec);
};

#endif