#include "PyramidAlignerAuto.hpp"

#include "FrameProvider.hpp"

using namespace std;
using namespace cv;

static std::vector<cv::Point2f> getCornerOnFrame(cv::Size size) {
  int rows = size.height;
  int cols = size.width;
  vector<Point2f> corner_frame(4);
  corner_frame[0] = Point2f(0, 0);
  corner_frame[1] = Point2f(cols, 0);
  corner_frame[2] = Point2f(cols, rows);
  corner_frame[3] = Point2f(0, rows);

  return corner_frame;
}
static std::vector<cv::Point2f> getCornerOnScene(cv::Size size, cv::Mat H) {
  vector<Point2f> corner_frame = getCornerOnFrame(size);

  vector<Point2f> corner_on_scene;
  perspectiveTransform(corner_frame, corner_on_scene, H);
  return corner_on_scene;
}
static cv::Rect getRectFromCorner(std::vector<cv::Point2f> &corner) {
  assert(!corner.empty());
  int x1 = corner[0].x;
  int y1 = corner[0].y;
  int x2 = corner[0].x;
  int y2 = corner[0].y;
  for (size_t i = 0; i < corner.size(); ++i) {
    if (corner[i].x < x1)
      x1 = corner[i].x;
    if (corner[i].x > x2)
      x2 = corner[i].x;
    if (corner[i].y < y1)
      y1 = corner[i].y;
    if (corner[i].y > y2)
      y2 = corner[i].y;
    // cout << corner[i] << endl;
  }
  return Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

PyramidAlignerAuto::PyramidAlignerAuto(string path_scene) {
  m_frame_provider = make_shared<FrameProvider>(path_scene, 0);
}

void PyramidAlignerAuto::align(cv::Mat frame, cv::Mat &trans, cv::Rect &rect) {
  //
  trans = Mat(3, 3, CV_32FC1, Scalar(0.0f));
}
void PyramidAlignerAuto::alignZncc(cv::Mat frame, cv::Mat refer, cv::Mat &trans,
                                   cv::Rect &rect) {

  Point p_center;
  vector<Point2f> frame_point;
  vector<Point2f> refer_point;
  // assert(alignZncc(frame, refer, p_center));
  p_center = Point(712, 458);
  frame_point.push_back(Point2f(frame.cols / 2, frame.rows / 2));
  refer_point.push_back(p_center);

#if 1
  int rows_step = frame.rows / 2;
  int cols_step = frame.cols / 2;
  for (int r = 0; r + rows_step <= frame.rows; r += rows_step) {
    for (int c = 0; c + cols_step <= frame.cols; c += cols_step) {
      cout << "HHH: " << r << " " << c << endl;
      Rect rect_frame(c, r, cols_step, rows_step);
      int x = p_center.x - frame.cols / 2 + c;
      int y = p_center.y - frame.rows / 2 + r;
      int width = cols_step;
      int height = rows_step;
      int pad_x = cols_step * 2 / 10;
      int pad_y = rows_step * 2 / 10;
      Rect rect_refer(x - pad_x, y - pad_y, width + 2 * pad_x,
                      height + 2 * pad_y);
      rect_refer = rect_refer & Rect(0, 0, refer.cols, refer.rows);
      Point p;
      assert(alignZncc(frame(rect_frame), refer(rect_refer), p));
      cout << rect_refer << endl;

      frame_point.push_back(Point2f(c + cols_step / 2, r + rows_step / 2));
      refer_point.push_back(Point2f(p.x + rect_refer.x, p.y + rect_refer.y));

      // cout << p << endl;
      // rectangle(refer, rect_refer, Scalar(255, 0, 0), 2);
      // Mat roi = refer(rect_refer);
      // circle(roi, p, 3, Scalar(255, 0, 0), -1);
      // imshow("refer", refer);

      // rectangle(frame, rect_frame, Scalar(255, 0, 0), 2);
      // imshow("frame", frame);
      // imshow("frame(rect_frame)", frame(rect_frame));
      // imshow("refer(rect_refer)", refer(rect_refer));
      // waitKey(0);

      // break;
    }
    // break;
  }
#endif
  for (size_t i = 0; i < frame_point.size(); ++i) {
    circle(refer, refer_point[i], 3, Scalar(255, 0, 0), -1);
    circle(frame, frame_point[i], 3, Scalar(255, 0, 0), -1);
  }
  // imshow("frame", frame);
  // imshow("refer", refer);
  // waitKey(0);

  trans = findHomography(frame_point, refer_point);
  rect = Rect(0, 0, refer.rows, refer.cols);

#if 0
  vector<Point2f> corner_scene = getCornerOnScene(frame.size(), trans);
  rect = getRectFromCorner(corner_scene);

  for (size_t i = 0; i < refer_point.size(); ++i) {
    refer_point[i].x -= rect.x;
    refer_point[i].y -= rect.y;
  }
  trans = findHomography(frame_point, refer_point);
#endif

  trans.convertTo(trans, CV_32FC1);
}
bool PyramidAlignerAuto::alignZncc(cv::Mat frame_, cv::Mat refer_,
                                   cv::Point &p) {
  float down_scale = 1;
  //
  Mat frame, refer;
  {
    Mat frame_down;
    resize(frame_, frame_down,
           Size(frame_.cols / down_scale, frame_.rows / down_scale));
    resize(refer_, refer,
           Size(refer_.cols / down_scale, refer_.rows / down_scale));
    int rows = frame_down.rows;
    int cols = frame_down.cols;
    if (rows % 2 == 0)
      rows++;
    if (cols % 2 == 0)
      cols++;
    frame = Mat(rows, cols, CV_8UC3, Scalar(0, 0, 0));
    frame_down.copyTo(frame(Rect(0, 0, frame_down.cols, frame_down.rows)));
  }
  // cout << frame.size() << endl;
  // imshow("frame", frame);
  // imshow("refer", refer);
  // waitKey(0);

  if (refer.rows < frame.rows || refer.cols < frame.cols) {
    return false;
  }

  cvtColor(frame, frame, CV_BGR2GRAY);
  cvtColor(refer, refer, CV_BGR2GRAY);
  frame.convertTo(frame, CV_32FC1);
  refer.convertTo(refer, CV_32FC1);

  Mat response(refer.size(), CV_32FC1, Scalar(0.0f));
  Rect rect_refer(0, 0, refer.cols, refer.rows);
  int r_begin = frame.rows / 2;
  int r_end = refer.rows - frame.rows / 2;
  int c_begin = frame.cols / 2;
  int c_end = refer.cols - frame.cols / 2;
  for (int r = r_begin; r < r_end; ++r) {
    cout << "r: " << r << " / " << refer.rows << endl;
    for (int c = c_begin; c < c_end; ++c) {
      Rect rect_frame(c - frame.cols / 2, r - frame.rows / 2, frame.cols,
                      frame.rows);
      Rect rect_overlap = rect_frame & rect_refer;
      // cout << rect_overlap << endl;
      Rect rect_overlap_on_frame = rect_overlap;
      rect_overlap_on_frame.x -= rect_frame.x;
      rect_overlap_on_frame.y -= rect_frame.y;
      Rect rect_overlap_on_refer = rect_overlap;
      Mat frame_roi = frame(rect_overlap_on_frame);
      Mat refer_roi = refer(rect_overlap_on_refer);
      // cout << frame_roi.size() << endl;
      // cout << refer_roi.size() << endl;
      float score = zncc(frame_roi, refer_roi);
      // cout << "score: " << score << endl;
      response.at<float>(r, c) = score;
      // break;
    }
    // break;
  }

  float response_min = 1e6;
  float response_max = -1e6;
  for (int r = r_begin; r < r_end; ++r) {
    for (int c = c_begin; c < c_end; ++c) {
      if (response.at<float>(r, c) > response_max) {
        response_max = response.at<float>(r, c);
      }
      if (response.at<float>(r, c) < response_min) {
        response_min = response.at<float>(r, c);
      }
    }
  }
  // cout << "response_min: " << response_min << endl;
  // cout << "response_max: " << response_max << endl;
  for (int r = r_begin; r < r_end; ++r) {
    for (int c = c_begin; c < c_end; ++c) {
      response.at<float>(r, c) = (response.at<float>(r, c) - response_min) /
                                 (response_max - response_min) * 255;
    }
  }
#if 0
  Mat response_;
  response.convertTo(response_, CV_8UC1);
  imshow("response_", response_);
  waitKey(0);
#endif

  float max_value = -1e6;
  Point max_point(-1, -1);

  for (int r = r_begin; r < r_end; ++r) {
    for (int c = c_begin; c < c_end; ++c) {
      if (response.at<float>(r, c) > max_value) {
        max_value = response.at<float>(r, c);
        max_point.x = c;
        max_point.y = r;
      }
    }
  }

  p = max_point;
  p.x *= down_scale;
  p.y *= down_scale;

  return true;
}
float PyramidAlignerAuto::zncc(const cv::Mat frame, const cv::Mat refer) {
  assert(frame.size() == refer.size());
  assert(frame.type() == refer.type());
  assert(frame.type() == CV_32FC1);
#if 0
  Mat frame_, refer_;
  frame.convertTo(frame_, CV_8UC1);
  refer.convertTo(refer_, CV_8UC1);
  imshow("frame_", frame_);
  imshow("refer_", refer_);
  waitKey(0);
#endif

  float frame_mean = 0;
  float refer_mean = 0;
  for (int r = 0; r < refer.rows; ++r) {
    for (int c = 0; c < refer.cols; ++c) {
      frame_mean += frame.at<float>(r, c);
      refer_mean += refer.at<float>(r, c);
    }
  }
  frame_mean /= refer.rows * refer.cols;
  refer_mean /= refer.rows * refer.cols;
  // cout << frame_mean << endl;
  // cout << refer_mean << endl;

  float sum_a = 0;
  float sum_b = 0;
  float sum_c = 0;
  for (int r = 0; r < refer.rows; ++r) {
    for (int c = 0; c < refer.cols; ++c) {
      float m = frame.at<float>(r, c) - frame_mean;
      float n = refer.at<float>(r, c) - refer_mean;
      sum_a += m * n;
      sum_b += m * m;
      sum_c += n * n;
    }
  }
  // cout << "sum_a: " << sum_a << endl;
  // cout << "sum_b: " << sum_b << endl;
  // cout << "sum_c: " << sum_c << endl;
  float gamma = sum_a / sqrt(sum_b * sum_c + 1e8);
  return gamma;
}
