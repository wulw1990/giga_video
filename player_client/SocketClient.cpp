#include "SocketClient.hpp"


using namespace std;

#include "Protocol.hpp"
#include "Transmitter.hpp"
#include "CameraSet.hpp"

SocketClient::SocketClient(std::string ip, int port ) {
	m_protocol = make_shared<Protocol>();
	m_transmitter = make_shared<Transmitter>();

	m_socket_id = m_transmitter->initSocketClient(ip, port);
	cout << "init ok" << endl;
}
void SocketClient::work() {
            CameraSet myCameras;
	while (1) {
		vector<Mat> imgs(10);  
		for(int i = 0; i < 5; i++)
		{
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
			imgs[i] = imdecode(raw, 1);
			//imshow("dec", dec);

			//waitKey(1);

			vector<unsigned char> send_data(2);
			vector<unsigned char> send_buf;
			m_protocol->encode(send_buf, "xyz", send_data);
			if ( ! m_transmitter->sendData(m_socket_id, send_buf) ) {
				goto CONNECT_END;
			}
		}
                        for (unsigned int i = 0; i < 5; i ++)
                        {
                            myCameras.getCapture(imgs[5 + i], i%myCameras.numCameras);    
                        }                        


		MultiImage_OneWin("Multiviews", imgs, cvSize(5,2), cvSize(288,162));
		cvWaitKey(1);

	}
CONNECT_END:
	cout << "connect end" << endl;
	m_transmitter->closeSocket(m_socket_id);
}

void SocketClient::MultiImage_OneWin(const std::string& MultiShow_WinName, const vector<Mat>& SrcImg_V, CvSize SubPlot, CvSize ImgMax_Size) 
{
	//Window's image  
    Mat Disp_Img;  
    //Width of source image  
    CvSize Img_OrigSize = cvSize(SrcImg_V[0].cols, SrcImg_V[0].rows);  
    //******************** Set the width for displayed image ********************//  
    //Width vs height ratio of source image  
    float WH_Ratio_Orig = Img_OrigSize.width/(float)Img_OrigSize.height;  
    CvSize ImgDisp_Size = cvSize(100, 100);  
    if(Img_OrigSize.width > ImgMax_Size.width)  
        ImgDisp_Size = cvSize(ImgMax_Size.width, (int)ImgMax_Size.width/WH_Ratio_Orig);  
    else if(Img_OrigSize.height > ImgMax_Size.height)  
        ImgDisp_Size = cvSize((int)ImgMax_Size.height*WH_Ratio_Orig, ImgMax_Size.height);  
    else  
        ImgDisp_Size = cvSize(Img_OrigSize.width, Img_OrigSize.height);  
    //******************** Check Image numbers with Subplot layout ********************//  
    int Img_Num = (int)SrcImg_V.size();  
    if(Img_Num > SubPlot.width * SubPlot.height)  
    {  
        cout<<"Your SubPlot Setting is too small !"<<endl;  
        exit(0);  
    }  
    //******************** Blank setting ********************//  
    CvSize DispBlank_Edge = cvSize(20, 15);  
    CvSize DispBlank_Gap  = cvSize(5, 5);  
    //******************** Size for Window ********************//  
    Disp_Img.create(Size(ImgDisp_Size.width*SubPlot.width + DispBlank_Edge.width + (SubPlot.width - 1)*DispBlank_Gap.width,   
        ImgDisp_Size.height*SubPlot.height + DispBlank_Edge.height + (SubPlot.height - 1)*DispBlank_Gap.height), CV_8UC3);  
    Disp_Img.setTo(0);//Background  
    //Left top position for each image  
    int EdgeBlank_X = (Disp_Img.cols - (ImgDisp_Size.width*SubPlot.width + (SubPlot.width - 1)*DispBlank_Gap.width))/2;  
    int EdgeBlank_Y = (Disp_Img.rows - (ImgDisp_Size.height*SubPlot.height + (SubPlot.height - 1)*DispBlank_Gap.height))/2;  
    CvPoint LT_BasePos = cvPoint(EdgeBlank_X, EdgeBlank_Y);  
    CvPoint LT_Pos = LT_BasePos;  
  
    //Display all images  
    for (int i=0; i < Img_Num; i++)  
    {  
        //Obtain the left top position  
        if ((i%SubPlot.width == 0) && (LT_Pos.x != LT_BasePos.x))  
        {  
            LT_Pos.x = LT_BasePos.x;  
            LT_Pos.y += (DispBlank_Gap.height + ImgDisp_Size.height);  
        }  
        //Writting each to Window's Image  
        Mat imgROI = Disp_Img(Rect(LT_Pos.x, LT_Pos.y, ImgDisp_Size.width, ImgDisp_Size.height));  
        resize(SrcImg_V[i], imgROI, Size(ImgDisp_Size.width, ImgDisp_Size.height));  
  
        LT_Pos.x += (DispBlank_Gap.width + ImgDisp_Size.width);  
    }  

     
    imshow(MultiShow_WinName.c_str(), Disp_Img);  
    cvWaitKey(1);   
}