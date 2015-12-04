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
	char recvline[4096], sendline[4096];
	while (1) {
		vector<unsigned char> recv_buf;
		if (!m_transmitter->readData(m_socket_id, recv_buf, m_protocol->getHeadLen())) {
			cout << "connect end" << endl;
			break;
		}
		// cout << recv_buf.size() << endl;
		// for (size_t i = 0; i < recv_buf.size(); ++i) {
		// 	cout << (int)recv_buf[i] << " ";
		// }
		// cout << endl;

		string cmd;
		int data_len;
		m_protocol->decodeHead(recv_buf, cmd, data_len);
		cout << "cmd: " << cmd << " data_len: " << data_len << endl;
		if (!m_transmitter->readData(m_socket_id, recv_buf, data_len)) {
			cout << "connect end" << endl;
			break;
		}

		Mat raw(1, recv_buf.size(), CV_8UC1, &recv_buf[0]);
		Mat dec = imdecode(raw, 1);
		imshow("dec", dec);
		waitKey(1);

		vector<unsigned char> send_data(2);
		vector<unsigned char> send_buf;
		m_protocol->encode(send_buf, "img", send_data);
		if ( ! m_transmitter->sendData(m_socket_id, send_buf) ) {
			printf("send msg error, end of this client");
			break;
		}



		// fgets(sendline, 4096, stdin);
		// vector<unsigned char> data;
		// // for(int i=0; i<4096; ++i){
		// // 	if(sendline[i]!=0){
		// // 		data.push_back(sendline[i]);
		// // 	}else{
		// // 		break;
		// // 	}
		// // }
		// data.resize(1024);
		// for(size_t i=0; i<data.size(); ++i){
		// 	data[i] = i % 256;
		// }
		// if ( ! m_transmitter->sendData(m_socket_id, data) ){
		// 	printf("send msg error");
		// 	exit(0);
		// }
		// cout << data.size() << endl;
	}
	m_transmitter->closeSocket(m_socket_id);
}