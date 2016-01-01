#include "SocketServer.hpp"

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#include "Protocol.hpp"
#include "Transmitter.hpp"
#include "CameraSet.hpp"

SocketServer::SocketServer(int port) {
    m_protocol = make_shared<Protocol>();
    m_transmitter = make_shared<Transmitter>();
    m_server_id = m_transmitter->initSocketServer(port);
}
void SocketServer::work()
{
    CameraSet myCamera;

    while (1) {
        //get a client
        cout << "waiting for a client" << endl;
        int client_id;
        if ( ! m_transmitter->getClientId(m_server_id, client_id) ) {
            continue;
        }
        //serve it
        // VideoCapture capture("/home/wuliwei/Videos/86-1.avi");
        /*
        VideoCapture capture("/home/bbnc/test/test.MOV");
        assert(capture.isOpened());
        */
        Mat frame;
        // while(capture.read(frame)){
        //     imshow("frame", frame);
        //     waitKey(33);
        // }
        Vector<Mat> imgs(10);  
        for (int i = 0; i < 10; i++)
        {
            stringstream ss;
            ss <<"/home/bbnc/picture/"<< i + 1 << ".png";
            imgs[i] = imread(ss.str());
        }
        unsigned int index = 0;
	unsigned int key = 0;
        while (key ++ < 1000) {
            //myCamera.getCapture(frame, index);
            index = (index + 1) % 10;
            
            vector<unsigned char> jpg;
            imencode(".jpg", imgs[index], jpg);
            cout << jpg.size() << endl;

            vector<unsigned char> send_buf;
            m_protocol->encode(send_buf, "img", jpg);
            if ( ! m_transmitter->sendData(client_id, send_buf) ) {
                goto CONNECT_END;
            }

            vector<unsigned char> recv_buf;
            if (!m_transmitter->readData(client_id, recv_buf, m_protocol->getHeadLen())) {
                goto CONNECT_END;
            }
            string cmd;
            int data_len;
            m_protocol->decodeHead(recv_buf, cmd, data_len);
            cout << "cmd: " << cmd << " data_len: " << data_len << endl;
            if (!m_transmitter->readData(client_id, recv_buf, data_len)) {
                goto CONNECT_END;
            }
        }
CONNECT_END:
        cout << "connect end" << endl;
        m_transmitter->closeSocket(client_id);
    }
    m_transmitter->closeSocket(m_server_id);
}

