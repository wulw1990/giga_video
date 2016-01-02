#ifndef CONSTRCUTOR_HPP_
#define CONSTRCUTOR_HPP_

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <opencv2/opencv.hpp>

class Constructor
{
public:
	Constructor();
	~Constructor(){}
	
	void writeInfoForAutopan(std::string path, int n_layers = 5, std::string name="info_scene.txt");

	bool constructVideo(std::string path_img, std::string raw_video_name, std::string path_out);
	
	// bool constructCameraSet(std::string path_out);
	

	void cutVideo(std::string name_src, std::string name_dst);

private:
};

#endif