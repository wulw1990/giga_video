#include "PyramidAlignerAuto.hpp"

#include "FrameProvider.hpp"

using namespace std;
using namespace cv;

PyramidAlignerAuto::PyramidAlignerAuto(string path_scene) {
  m_frame_provider = make_shared<FrameProvider>(path_scene, 0);
}

void PyramidAlignerAuto::align(cv::Mat frame, cv::Mat &trans, cv::Rect &rect) {
  //
  trans = Mat(3, 3, CV_32FC1, Scalar(0.0f));
}
void PyramidAlignerAuto::alignZncc(cv::Mat frame_, cv::Mat refer,
                                   cv::Mat &trans, cv::Rect &rect) {
  //
  Mat frame;
  {
    int rows = frame_.rows;
    int cols = frame_.cols;
    if (rows % 2 == 0)
      rows++;
    if (cols % 2 == 0)
      cols++;
    frame = Mat(rows, cols, CV_8UC3, Scalar(0, 0, 0));
    frame_.copyTo(frame(Rect(0, 0, frame_.cols, frame_.rows)));
  }
  cout << frame.size() << endl;
  imshow("frame", frame);
  waitKey(0);
}
