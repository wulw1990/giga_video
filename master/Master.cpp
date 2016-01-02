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

const string win_title = "giga player";
const int w = 1700;
const int h = 900;

// #define ENABLE_APP

Master::Master(std::string path, int port) {
    m_protocol = make_shared<Protocol>();
    m_transmitter = make_shared<Transmitter>();
    m_server_id = m_transmitter->initSocketServer(port);

    m_info.m_frame_provider = make_shared<FrameProvider>(path, true);

#ifndef ENABLE_APP
    int n_layers = m_info.m_frame_provider->getNumLayers();
    Size top_layer_size(m_info.m_frame_provider->getLayerWidth(n_layers - 1), m_info.m_frame_provider->getLayerHeight(n_layers - 1));
    Size winsize(1000, 500);
    m_info.m_window_controller = make_shared<WindowController>(n_layers, top_layer_size, winsize);
    m_info.update = true;
#endif
}
cv::Mat Master::readMatFromSocket(int socket_id, int color, string error) {
    vector<unsigned char> recv_buf;
    if (!m_transmitter->readData(socket_id, recv_buf, m_protocol->getHeadLen())) {
        cerr << error << endl;
        exit(-1);
    }
    string cmd;
    int data_len;
    m_protocol->decodeHead(recv_buf, cmd, data_len);
    // cout << "cmd: " << cmd << " data_len: " << data_len << endl;
    if (!m_transmitter->readData(socket_id, recv_buf, data_len)) {
        cerr << error << endl;
        exit(-1);
    }

    // for(int i=0; i<=255; ++i)
    //               cout << i << "\t" << (int)recv_buf[i] << endl;
    //           exit(-1);

    Mat raw(1, recv_buf.size(), CV_8UC1, &recv_buf[0]);
    Mat dec = imdecode(raw, color);
    return dec;
}
cv::Mat Master::getFrame() {
    double x, y, z;
    m_info.m_window_controller->getXYZ(x, y, z);

    vector<float> whxyz;
    whxyz.push_back(w);
    whxyz.push_back(h);
    whxyz.push_back(x);
    whxyz.push_back(y);
    whxyz.push_back(z);

    vector<unsigned char> send_buf;
    m_protocol->encodeFloatVector(whxyz, send_buf);

    Mat frame, mask;
    m_info.m_frame_provider->getFrameWithMask(frame, mask, w, h, x, y, z);

    for (size_t i = 0; i < m_slave_id.size(); ++i) {
        string error = "slave No." + to_string(i) + " dead!";
        if ( ! m_transmitter->sendData(m_slave_id[i], send_buf) ) {
            cerr << error << endl;
            exit(-1);
        }

        Mat frame1 = readMatFromSocket(m_slave_id[i], 1, error);
        Mat mask1 = readMatFromSocket(m_slave_id[i], 0, error);
        // cout << frame.size() << endl;
        // cout << mask.size() << endl;
        // imshow("hh", frame);
        // imshow("hh-mask", mask);

        frame1.copyTo(frame, mask1);
    }

    return frame;
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
        } else {
            cout << "Hello, my slave No." << m_slave_id.size() << endl;
            m_slave_id.push_back(id);
        }
    }

    cout << endl;
    cout << "============waiting for client request============" << endl;

    // double x = 0.5;
    // double y = 0.5;
    // double z = 4;
    // Mat frame, mask;
    // m_frame_provider->getFrameWithMask(frame, mask, 1000.0, 500.0, x, y, z);
    // imshow("frame", frame);
    // imshow("mask", mask);
    // waitKey(0);

    while (1) {
#ifdef ENABLE_APP
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
#else
        // if (m_info.update) {
        Mat frame = getFrame();
        imshow(win_title, frame);
        cv::setMouseCallback(win_title, onMouse, &m_info);
        m_info.update = false;
        // }
        waitKey(33);
#endif
    }
    m_transmitter->closeSocket(m_server_id);
}

void Master::onMouse(int event, int x, int y, int, void* data)
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
        if (abs(info->pre_x - x) > 5 || abs(info->pre_y - y) > 5) {
            info->m_window_controller->move(info->pre_x - x, info->pre_y - y);
            info->update = true;
        }
        break;
    case EVENT_LBUTTONDBLCLK:
        info->m_window_controller->zoom(1);
        info->update = true;
        break;
    case EVENT_RBUTTONDBLCLK:
        info->m_window_controller->zoom(-1);
        info->update = true;
        break;
    }
}