#ifndef TRANSMITTER_HPP_
#define TRANSMITTER_HPP_

#include <string>
#include <vector>

class Transmitter
{
public:
	Transmitter(){}
	~Transmitter(){}

	int initSocketServer(int port);
	int initSocketClient( std::string ip, int port);
	bool getClientId(int server_id, int& client_id);
	void closeSocket(int socket_id);


	bool sendData(int socket_id, const std::vector<unsigned char>& data);
	bool readData(int socket_id, std::vector<unsigned char>& data, int len);

};

#endif