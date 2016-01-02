#include "Master.hpp"

#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#include "Protocol.hpp"
#include "Transmitter.hpp"
#include "DataProvide.hpp"
#include "WindowController.hpp"

const int SLAVE_NUM = 2;

Master::Master(std::string path, int port) {
    m_protocol = make_shared<Protocol>();
    m_transmitter = make_shared<Transmitter>();
    m_server_id = m_transmitter->initSocketServer(port);

    m_frame_provider = make_shared<FrameProvider>(path, true);
}
void Master::work()
{
    cout << endl;
    cout << "============waiting for slave request============" << endl;
    while ((int)m_slave_id.size() < SLAVE_NUM) {
        cout << endl;
        cout << "----------waiting for a slave------------" << endl;
        int id;
        if ( ! m_transmitter->getClientId(m_server_id, id) ) {
            continue;
        }else{
            cout << "Hello, my slave No." << m_slave_id.size() << endl;
            m_slave_id.push_back(id);
        }
    }

    cout << endl;
    cout << "============waiting for client request============" << endl;

    double x = 0.5;
    double y = 0.5;
    double z = 4;
    Mat frame, mask;
    m_frame_provider->getFrameWithMask(frame, mask, 1000.0, 500.0, x, y, z);
    imshow("frame", frame);
    imshow("mask", mask);
    waitKey(0);

    while (1) {
        //get a client
        cout << endl;
        cout << "----------waiting for a client------------" << endl;
        int client_id;
        if ( ! m_transmitter->getClientId(m_server_id, client_id) ) {
            continue;
        }
        cout << "Welcome, I have " << SLAVE_NUM << " slave!" << endl;

// CONNECT_END:
        cout << "connect end" << endl;
        m_transmitter->closeSocket(client_id);
    }
    m_transmitter->closeSocket(m_server_id);
}

