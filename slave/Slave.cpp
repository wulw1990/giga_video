#include "Slave.hpp"

#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#include "Protocol.hpp"
#include "Transmitter.hpp"
#include "DataProvide.hpp"

Slave::Slave(std::string path, std::string ip, int port, std::string name ) {
	m_protocol = make_shared<Protocol>();
	m_transmitter = make_shared<Transmitter>();

	m_socket_id = m_transmitter->initSocketClient(ip, port);
	cout << "init ok" << endl;

	m_name = name;

	m_frame_provider = make_shared<FrameProvider>(path, true);
}
static void printVector(string name, vector<float> vec) {
	cout << name << ": ";
	for (size_t i = 0; i < vec.size(); ++i) {
		cout << vec[i] << " ";
	}
	cout << endl;
}
void Slave::work() {
	int dx = 0;
	int dy = 0;
	int dz = 0;

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

		vector<float> fvec;
		m_protocol->decodeFloatVector(recv_buf, fvec);

		printVector("whxyz: ", fvec);


		double w = fvec[0];
		double h = fvec[1];
		double x = fvec[2];
		double y = fvec[3];
		double z = fvec[4];
		Mat frame, mask;
		m_frame_provider->getFrameWithMask(frame, mask, w, h, x, y, z);

		vector<unsigned char> jpg;
		vector<unsigned char> send_buf;

		imencode(".jpg", frame, jpg);
		m_protocol->encode(send_buf, "img", jpg);
		if ( ! m_transmitter->sendData(m_socket_id, send_buf) ) {
			goto CONNECT_END;
		}

		imencode(".jpg", mask, jpg);
		m_protocol->encode(send_buf, "img", jpg);
		if ( ! m_transmitter->sendData(m_socket_id, send_buf) ) {
			goto CONNECT_END;
		}

	}
CONNECT_END:
	cout << "connect end" << endl;
	m_transmitter->closeSocket(m_socket_id);
}