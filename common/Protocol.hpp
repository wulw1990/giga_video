#ifndef PROTOCOL_HPP_
#define PROTOCOL_HPP_

#include <vector>
#include <string>

class Protocol
{

	public:
		Protocol();

		void encode(
		    std::vector<unsigned char>& buf,
		    std::string cmd, const std::vector<unsigned char>& data );

		void decodeHead(
		    const std::vector<unsigned char>& buf,
		    std::string& cmd, int& data_len );

		void encodeXYZ(
		    std::vector<unsigned char>& buf,
		    int x, int y, int z);

		void decodeDataXYZ(
		    const std::vector<unsigned char>& buf,
		    int& x, int& y, int& z);



		int getHeadLen() {
			return CMD_LEN + LEN_LEN;
		}


	private:
		const int CMD_LEN;
		const int LEN_LEN;

		void int2Vector(int value, std::vector<unsigned char>& vec);
		void vector2Int(int& value, const std::vector<unsigned char>& vec);
};

#endif