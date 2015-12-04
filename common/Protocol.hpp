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
		void decode(
			const std::vector<unsigned char>& buf,
			std::string& cmd, std::vector<unsigned char>& data );

	private:
		const int CMD_LEN;
		const int LEN_LEN;
		const int DATA_LEN;
};

#endif