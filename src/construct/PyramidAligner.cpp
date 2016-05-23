#include "PyramidAligner.hpp"

#include "FrameProvider.hpp"

using namespace std;
using namespace cv;

PyramidAligner::PyramidAligner(string path_scene) {
  m_frame_provider = make_shared<FrameProvider>(path_scene, 0);
}
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
void PyramidAligner::align(cv::Mat frame, cv::Mat &trans, cv::Rect &rect) {
  int layer_id = 0;
  int layer_width = m_frame_provider->getLayerWidth(layer_id);
  int layer_height = m_frame_provider->getLayerHeight(layer_id);
  Rect rect_global(0, 0, layer_width, layer_height);

  for (layer_id = 0; layer_id < 2; ++layer_id) {
    Mat refer = m_frame_provider->getFrameBackground(rect_global, layer_id);
    cout << refer.size() << endl;
    Rect rect_local;
    alignRect(frame, refer, rect_local);
    rect_local.x += rect_global.x;
    rect_local.y += rect_global.y;
    rect_global = rect_local;

    rect_global.x *= 2;
    rect_global.y *= 2;
    rect_global.width *= 2;
    rect_global.height *= 2;
  }

  cout << rect_global << endl;
  Mat refer = m_frame_provider->getFrameBackground(rect_global, layer_id);
  alignTransRect(frame, refer, trans, rect);
  rect.x += rect_global.x;
  rect.y += rect_global.y;
  {
    float scale = pow(2.0, m_frame_provider->getNumLayers() - 1 - layer_id);
    cout << scale << endl;
    cout << trans << endl;
    trans.at<float>(0, 0) *= scale;
    trans.at<float>(0, 1) *= scale;
    trans.at<float>(0, 2) *= scale;
    trans.at<float>(1, 0) *= scale;
    trans.at<float>(1, 1) *= scale;
    trans.at<float>(1, 2) *= scale;
    cout << trans << endl;
    rect.x *= scale;
    rect.y *= scale;
    rect.width *= scale;
    rect.height *= scale;
  }
}
bool PyramidAligner::alignRect(cv::Mat frame, cv::Mat refer, cv::Rect &rect) {
  Mat show_frame;
  Mat show_refer;
  getShowImage(frame, show_frame);
  float scale_refer = getShowImage(refer, show_refer);
  //   cout << "scale_refer: " << scale_refer << endl;
  //   cout << "show_refer: " << show_refer.size() << endl;

  MouseRectInfo info;
  info.title = "show_refer";
  info.image = show_refer;
  info.rect = Rect(-1, -1, -1, -1);
  info.move = false;
  imshow("show_frame", show_frame);
  imshow(info.title, show_refer);
  setMouseCallback(info.title, onMouseRect, &info);
  while (1) {
    char key = waitKey(33);
    if (key == 'q')
      break;
  }
  cvDestroyWindow("show_refer");
  cvDestroyWindow("show_frame");

  rect = info.rect;
  //   cout << rect << endl;
  rect.x *= scale_refer;
  rect.y *= scale_refer;
  rect.width *= scale_refer;
  rect.height *= scale_refer;
  //   cout << rect << endl;

  bool success = rect.x > 0 && rect.y > 0 && rect.width > 0 && rect.height > 0;
  return success;
}
bool PyramidAligner::alignTransRect(cv::Mat frame, cv::Mat refer,
                                    cv::Mat &trans, cv::Rect &rect) {
  //
  Mat show_frame;
  Mat show_refer;
  float scale_frame = getShowImage(frame, show_frame);
  float scale_refer = getShowImage(refer, show_refer);
  cout << "show_frame: " << show_frame.size() << endl;
  cout << "show_refer: " << show_refer.size() << endl;

  MousePointInfo info_frame;
  info_frame.title = "frame";
  info_frame.image = show_frame;
  info_frame.point.clear();
  imshow(info_frame.title, info_frame.image);
  setMouseCallback(info_frame.title, onMousePoint, &info_frame);

  MousePointInfo info_refer;
  info_refer.title = "refer";
  info_refer.image = show_refer;
  info_refer.point.clear();
  imshow(info_refer.title, info_refer.image);
  setMouseCallback(info_refer.title, onMousePoint, &info_refer);

  while (1) {
    char key = waitKey(33);
    if (key == 'q')
      break;
  }

  cvDestroyWindow(info_frame.title.c_str());
  cvDestroyWindow(info_refer.title.c_str());

  cout << info_frame.point.size() << endl;
  cout << info_refer.point.size() << endl;

  if (info_frame.point.size() != info_frame.point.size()) {
    return false;
  }

  for (size_t i = 0; i < info_frame.point.size(); ++i) {
    info_frame.point[i].x *= scale_frame;
    info_frame.point[i].y *= scale_frame;
    info_refer.point[i].x *= scale_refer;
    info_refer.point[i].y *= scale_refer;
  }
  trans = findHomography(info_frame.point, info_refer.point);
  vector<Point2f> corner_scene = getCornerOnScene(frame.size(), trans);
  rect = getRectFromCorner(corner_scene);

  for (size_t i = 0; i < info_refer.point.size(); ++i) {
    info_refer.point[i].x -= rect.x;
    info_refer.point[i].y -= rect.y;
  }
  trans = findHomography(info_frame.point, info_refer.point);
  trans.convertTo(trans, CV_32FC1);

  return true;
}
float PyramidAligner::getShowImage(cv::Mat src, cv::Mat &dst) {
  const int w_best = 1800;
  const int h_best = 900;

  float scale = max((float)src.cols / w_best, (float)src.rows / h_best);
  scale = max(scale, 1.0f);
  resize(src, dst, Size(src.cols / scale, src.rows / scale));
  return scale;
}
void PyramidAligner::onMouseRect(int event, int x, int y, int, void *data) {
  if (event != EVENT_LBUTTONDOWN && event != EVENT_LBUTTONUP &&
      event != EVENT_MOUSEMOVE)
    return;
  MouseRectInfo *p_info = (MouseRectInfo *)data;
  Mat show = p_info->image.clone();
  switch (event) {
  case EVENT_LBUTTONDOWN:
    p_info->rect.x = x;
    p_info->rect.y = y;
    p_info->rect.width = 0;
    p_info->rect.height = 0;
    p_info->move = true;
    break;
  case EVENT_LBUTTONUP:
    p_info->rect.width = x - p_info->rect.x;
    p_info->rect.height = y - p_info->rect.y;
    p_info->move = false;
    break;
  case EVENT_MOUSEMOVE:
    if (p_info->move) {
      p_info->rect.width = x - p_info->rect.x;
      p_info->rect.height = y - p_info->rect.y;
    }
    break;
  }
  if (p_info->rect.width > 0 && p_info->rect.x >= 0)
    rectangle(show, p_info->rect, Scalar(255, 0, 0), 2);
  imshow(p_info->title, show);
  waitKey(1);
}

void PyramidAligner::onMousePoint(int event, int x, int y, int, void *data) {
  if (event != EVENT_LBUTTONDOWN)
    return;
  MousePointInfo *p_info = (MousePointInfo *)data;
  Mat show = p_info->image.clone();
  switch (event) {
  case EVENT_LBUTTONDOWN:
    if (!existPoint(Point2f(x, y), p_info->point)) {
      p_info->point.push_back(Point(x, y));
    }
    break;
  }
  for (size_t i = 0; i < p_info->point.size(); ++i) {
    circle(show, p_info->point[i], 3, Scalar(255, 0, 0), -3);
  }
  imshow(p_info->title, show);
  waitKey(1);
}
bool PyramidAligner::existPoint(cv::Point2f p, std::vector<cv::Point2f> &v) {
  float T = 5;
  for (size_t i = 0; i < v.size(); ++i) {
    cv::Point q = v[i];
    if (abs(p.x - q.x) < T && abs(p.y - q.y) < T)
      return true;
  }
  return false;
}
