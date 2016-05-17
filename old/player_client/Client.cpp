#include "Client.hpp"

#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#include "Protocol.hpp"
#include "Transmitter.hpp"

Client::Client(std::string ip, int port ) {
	m_protocol = make_shared<Protocol>();
	m_transmitter = make_shared<Transmitter>();

	m_socket_id = m_transmitter->initSocketClient(ip, port);
	cout << "init ok" << endl;

	m_info.pre_x = -1;
	m_info.pre_y = -1;
	m_info.dx = 0;
	m_info.dy = 0;
	m_info.dz = 0;
	m_info.update = false;

}
void Client::work() {
	int dx = 0;
	int dy = 0;
	int dz = 0;

	//send window size
	vector<unsigned char> send_buf;
	m_protocol->encodeXYZ(send_buf, 1700, 900, 0);
	if ( ! m_transmitter->sendData(m_socket_id, send_buf) ) {
		goto CONNECT_END;
	}

	while (1) {
		vector<unsigned char> recv_buf;
		if (!m_transmitter->readData(m_socket_id, recv_buf, m_protocol->getHeadLen())) {
			goto CONNECT_END;
		}

		string cmd;
		int data_len;
		m_protocol->decodeHead(recv_buf, cmd, data_len);
		// cout << "cmd: " << cmd << " data_len: " << data_len << endl;
		if (!m_transmitter->readData(m_socket_id, recv_buf, data_len)) {
			goto CONNECT_END;
		}

		// for(int i=0; i<=255; ++i)
  //               cout << i << "\t" << (int)recv_buf[i] << endl;
  //           exit(-1);

		Mat raw(1, recv_buf.size(), CV_8UC1, &recv_buf[0]);
		Mat dec = imdecode(raw, 1);
		imshow("client", dec);
		cv::setMouseCallback("client", onMouse, &m_info);
		waitKey(30);

		dx = 0;
		dy = 0;
		dz = 0;
		if (m_info.update) {
			dx = m_info.dx;
			dy = m_info.dy;
			dz = m_info.dz;
			// cout << dx << " " << dy << " " << dz << endl;
			m_info.update = false;
			m_info.dx = 0;
			m_info.dy = 0;
			m_info.dz = 0;
		}
		vector<unsigned char> send_buf;
		// vector<unsigned char> send_data(30);
		// m_protocol->encode(send_buf, "jpg", send_data);
		m_protocol->encodeXYZ(send_buf, dx, dy, dz);
		if ( ! m_transmitter->sendData(m_socket_id, send_buf) ) {
			goto CONNECT_END;
		}

	}
CONNECT_END:
	cout << "connect end" << endl;
	m_transmitter->closeSocket(m_socket_id);
}
void Client::onMouse(int event, int x, int y, int, void* data)
{
	if (event != EVENT_LBUTTONDOWN
	        && event != EVENT_LBUTTONUP
	        && event != EVENT_LBUTTONDBLCLK
	        && event != EVENT_RBUTTONDBLCLK)
		return;
	Info* info = (Info*)data;
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		info->pre_x = x;
		info->pre_y = y;
		break;
	case EVENT_LBUTTONUP:
		// cout << "x: " << info->pre_x << "-" << x << endl;
		// cout << "y: " << info->pre_y << "-" << y << endl;
		info->dx = info->pre_x - x;
		info->dy = info->pre_y - y;
		info->dz = 0;
		break;
	case EVENT_LBUTTONDBLCLK:
		info->dx = 0;
		info->dy = 0;
		info->dz = 1;
		break;
	case EVENT_RBUTTONDBLCLK:
		info->dx = 0;
		info->dy = 0;
		info->dz = -1;
		break;
	}
	info->update = true;
}