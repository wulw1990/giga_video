#include "SocketServer.hpp"

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#include "Protocol.hpp"
#include "Transmitter.hpp"

SocketServer::SocketServer(int port) {
    m_protocol = make_shared<Protocol>();
    m_transmitter = make_shared<Transmitter>();
    m_server_id = m_transmitter->initSocketServer(port);
}
void SocketServer::work()
{
    while (1) {
        //get a client
        cout << "waiting for a client" << endl;
        int client_id;
        if ( ! m_transmitter->getClientId(m_server_id, client_id) ) {
            continue;
        }
        //serve it
        VideoCapture capture("/home/wuliwei/Videos/86-1.avi");
        assert(capture.isOpened());
        Mat frame;
        // while(capture.read(frame)){
        //     imshow("frame", frame);
        //     waitKey(33);
        // }


        while (capture.read(frame)) {
            vector<unsigned char> jpg;
            imencode(".jpg", frame, jpg);
            cout << jpg.size() << endl;

            vector<unsigned char> send_buf;
            m_protocol->encode(send_buf, "img", jpg);
            if ( ! m_transmitter->sendData(client_id, send_buf) ) {
                printf("send msg error, end of this client");
                break;
            }

            vector<unsigned char> recv_buf;
            if (!m_transmitter->readData(client_id, recv_buf, m_protocol->getHeadLen())) {
                cout << "connect end" << endl;
                break;
            }
            string cmd;
            int data_len;
            m_protocol->decodeHead(recv_buf, cmd, data_len);
            cout << "cmd: " << cmd << " data_len: " << data_len <<endl;
            if (!m_transmitter->readData(client_id, recv_buf, data_len)) {
                cout << "connect end" << endl;
                break;
            }
        }
        // exit(-1);
        // while (1) {
        //     vector<unsigned char> data;
        //     if(!m_transmitter->readData(client_id, data, 1024)){
        //         cout << "connect end" << endl;
        //         break;
        //     }
        //     cout << data.size() << endl;
        //     cout << (int)data[0] << " " << (int)data[data.size()-1] << endl;
        // }

        //close the client connection, wait for another client
        m_transmitter->closeSocket(client_id);
    }
    m_transmitter->closeSocket(m_server_id);
}

