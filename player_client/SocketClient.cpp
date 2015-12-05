#include "SocketClient.hpp"

#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#include "Protocol.hpp"
#include "Transmitter.hpp"

SocketClient::SocketClient(std::string ip, int port ) {
	m_protocol = make_shared<Protocol>();
	m_transmitter = make_shared<Transmitter>();

	m_socket_id = m_transmitter->initSocketClient(ip, port);
	cout << "init ok" << endl;
}
void SocketClient::work() {
	while (1) {
		vector<unsigned char> recv_buf;
		if (!m_transmitter->readData(m_socket_id, recv_buf, m_protocol->getHeadLen())) {
			goto CONNECT_END;
		}

		string cmd;
		int data_len;
		m_protocol->decodeHead(recv_buf, cmd, data_len);
		cout << "cmd: " << cmd << " data_len: " << data_len << endl;
		if (!m_transmitter->readData(m_socket_id, recv_buf, data_len)) {
			goto CONNECT_END;
		}

		Mat raw(1, recv_buf.size(), CV_8UC1, &recv_buf[0]);
		Mat dec = imdecode(raw, 1);
		imshow("dec", dec);
		waitKey(1);

		vector<unsigned char> send_data(2);
		vector<unsigned char> send_buf;
		m_protocol->encode(send_buf, "xyz", send_data);
		if ( ! m_transmitter->sendData(m_socket_id, send_buf) ) {
			goto CONNECT_END;
		}
	}
CONNECT_END:
	cout << "connect end" << endl;
	m_transmitter->closeSocket(m_socket_id);
}