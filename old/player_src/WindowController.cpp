#include "WindowController.hpp"

using namespace std;
using namespace cv;

WindowController::WindowController( int n_layers, cv::Size top_layer_size, cv::Size winsize){
	assert(n_layers>0);
	m_layer_size.resize(n_layers);
	m_layer_size[n_layers-1] = top_layer_size;
	for(int i=n_layers-2; i>=0; --i){
		m_layer_size[i].width = m_layer_size[i+1].width / 2;
		m_layer_size[i].height = m_layer_size[i+1].height / 2;
	}

	m_winsize = winsize;

	int init_layer = -1.0;
	for(int i=n_layers-1; i>=0; --i){
		if(m_layer_size[i].width<winsize.width && m_layer_size[i].height<winsize.height){
			init_layer = i;
			break;
		}
	}
	x = 0.5;
	y = 0.5;
	z = (double)init_layer;
}
void WindowController::getXYZ(double&x, double& y, double& z){
	x = this->x;
	y = this->y;
	z = this->z;
}
void WindowController::move(int dx, int dy){

	int top_layer_id = int(m_layer_size.size()-1);
	Size top_layer_size = m_layer_size[top_layer_id];

	double cur_zoom_width = pow(2, z - top_layer_id) * top_layer_size.width;
	double cur_zoom_height = pow(2, z - top_layer_id) * top_layer_size.height;

	// cout << z << endl;
	// cout << cur_layer_width << "\t" << cur_layer_height << endl;

	double fdx = (double) dx / cur_zoom_width;
	double fdy = (double) dy / cur_zoom_height;

	x += fdx;
	y += fdy;

	x = max(x, 0.0);
	x = min(x, 1.0);
	y = max(y, 0.0);
	y = min(y, 1.0);
}
void WindowController::zoom(double dz){
	z += dz;
	double top_layer_id = double(m_layer_size.size()-1);
	z = max(z, -4.0);
	z = min(z, top_layer_id);
}