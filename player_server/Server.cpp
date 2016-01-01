#include "Server.hpp"

#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
#include "Protocol.hpp"
#include "Transmitter.hpp"
#include "DataProvide.hpp"
#include "WindowController.hpp"

Server::Server(std::string path, int port) {
    m_protocol = make_shared<Protocol>();
    m_transmitter = make_shared<Transmitter>();
    m_server_id = m_transmitter->initSocketServer(port);

    m_frame_provider = make_shared<FrameProvider>(path, true);


}
void Server::work()
{
    while (1) {
        //get a client
        cout << "waiting for a client" << endl;
        int client_id;
        if ( ! m_transmitter->getClientId(m_server_id, client_id) ) {
            continue;
        }
        cout << "serve it" << endl;
        //serve it

        Size winsize(1000, 500);
        {
            vector<unsigned char> recv_buf;
            if (!m_transmitter->readData(client_id, recv_buf, m_protocol->getHeadLen())) {
                cout << "connect end" << endl;
                break;
            }
            string cmd;
            int data_len;
            m_protocol->decodeHead(recv_buf, cmd, data_len);
            // cout << "cmd: " << cmd << " data_len: " << data_len << endl;
            if (!m_transmitter->readData(client_id, recv_buf, data_len)) {
                cout << "connect end" << endl;
                break;
            }
            int dx, dy, dz;
            m_protocol->decodeDataXYZ(recv_buf, dx, dy, dz);
            winsize.width = dx;
            winsize.height = dy;
        }
        cout << "width: " << winsize.width << " height: " << winsize.height << endl;

        //get window size
        int n_layers = m_frame_provider->getNumLayers();
        Size top_layer_size(m_frame_provider->getLayerWidth(n_layers - 1), m_frame_provider->getLayerHeight(n_layers - 1));
        m_window_controller = make_shared<WindowController>(n_layers, top_layer_size, winsize);

        // m_window_controller->zoom(4);
        // m_window_controller->move(-1000, 500);

        while (1) {
            double x, y, z;
            m_window_controller->getXYZ(x, y, z);
            Mat frame = m_frame_provider->getFrame(winsize.width, winsize.height, x, y, z);

            vector<unsigned char> jpg;
            imencode(".jpg", frame, jpg);
            // cout << jpg.size() << endl;

            // cout << "cmd: " << "img" << " data_len: " << jpg.size() << endl;
            // for(int i=0; i<=255; ++i){
            //     jpg[i] = i;
            // }
            // cout << (int)jpg[0] << endl;
            // cout << (int)jpg[255] << endl;


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
            // cout << "cmd: " << cmd << " data_len: " << data_len << endl;
            if (!m_transmitter->readData(client_id, recv_buf, data_len)) {
                goto CONNECT_END;
            }
            int dx, dy, dz;
            m_protocol->decodeDataXYZ(recv_buf, dx, dy, dz);
            if (dx != 0 || dy != 0 || dz != 0) {
                cout << dx << " " << dy << " " << dz << endl;
                // for (size_t i = 0; i < recv_buf.size(); ++i) {
                //     cout << (int)recv_buf[i] << " ";
                // }
                // cout << endl;
            }
            // cout << dx << " " << dy << " " << dz << endl;
            m_window_controller->move(dx, dy);
            m_window_controller->zoom(dz);

        }
CONNECT_END:
        cout << "connect end" << endl;
        m_transmitter->closeSocket(client_id);
    }
    m_transmitter->closeSocket(m_server_id);
}

