#ifndef SOCKET_CLIENT_HPP_
#define SOCKET_CLIENT_HPP_

#include <string>
#include <memory>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;
class Protocol;
class Transmitter;

class SocketClient
{
public:
	SocketClient(std::string ip, int port);
	~SocketClient(){}
	void work();
	void MultiImage_OneWin(const std::string& MultiShow_WinName, const vector<Mat>& SrcImg_V, CvSize SubPlot, CvSize ImgMax_Size = cvSize(400, 280));

private:
	std::shared_ptr<Protocol> m_protocol;
	std::shared_ptr<Transmitter> m_transmitter;
	int m_socket_id;
};

#endif