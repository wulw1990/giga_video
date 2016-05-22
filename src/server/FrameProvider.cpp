#include "FrameProvider.hpp"

using namespace std;
using namespace cv;

#include "TileProvider.hpp"
#include "VideoProvider.hpp"
#include "Timer.hpp"

const Scalar default_color(127, 127, 127);

FrameProvider::FrameProvider(std::string path, int video_mode) {
  m_tile_provider = make_shared<TileProvider>(path, path + "info_scene.txt");

  m_video_mode = video_mode;
  if (m_video_mode == 1) {
    m_video_provider = make_shared<VideoProvider>(path + "video/");
  } else if (m_video_mode == 2) {
    m_video_provider = make_shared<VideoProvider>(path + "video/", true);
  } else {
    cout << "unsupported video_mode:" << video_mode << endl;
    exit(-1);
  }
  cout << "video_mode: " << m_video_mode << endl;
}

cv::Mat FrameProvider::getFrameBackground(int w, int h, double x, double y,
                                          double z) {
  // calculate dst w, h, x, y, z
  int source_layer_id = getNearestLayer(z);
  int sx, sy, sw, sh;
  calculateSourceWindow(w, h, x, y, z, source_layer_id, &sw, &sh, &sx, &sy);

  cv::Mat frame = getFrameBackground(sw, sh, sx, sy, source_layer_id);
  resize(frame, frame, Size(w, h));
  return frame;
}
cv::Mat FrameProvider::getFrameBackground(int w, int h, int x, int y, int z) {
  // cout << "z: " << z << endl;
  Timer timer;
  const int LEN = m_tile_provider->getTileLen();
  Rect rect(x, y, w, h);
  Mat frame = Mat(h, w, CV_8UC3, default_color);
  Mat mask = Mat(h, w, CV_8UC1, Scalar(0));

  int X1 = x / LEN;
  int Y1 = y / LEN;
  int X2 = (x + w - 1) / LEN;
  int Y2 = (y + h - 1) / LEN;

  vector<Mat> vtile;
  {
    const int ROWS = m_tile_provider->getRowsOfLayer(z);
    const int COLS = m_tile_provider->getColsOfLayer(z);
    vector<int> vx, vy, vz;
    for (int x = X1; x <= X2; ++x) {
      for (int y = Y1; y <= Y2; ++y) {
        if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
          vx.push_back(x);
          vy.push_back(y);
          vz.push_back(z);
        }
      }
    }
    m_tile_provider->getTile(vx, vy, vz, vtile);
  }

  for (int i = 0, x = X1; x <= X2; ++x) {
    for (int y = Y1; y <= Y2; ++y) {
      Mat tile;
      const int ROWS = m_tile_provider->getRowsOfLayer(z);
      const int COLS = m_tile_provider->getColsOfLayer(z);
      if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
        tile = vtile[i++];
      } else {
        tile = Mat(LEN, LEN, CV_8UC3, default_color);
      }
      Rect tile_rect(x * LEN, y * LEN, LEN, LEN);
      copyMatToMat(tile, tile_rect, frame, rect);
    }
  }
  return frame;
}

bool FrameProvider::hasFrameForeground(int w, int h, double x, double y,
                                       double z) {
  if (m_video_mode == 0) {
    return false;
  }
  // calculate dst w, h, x, y, z
  int source_layer_id = getNearestLayer(z);
  int sx, sy, sw, sh;
  calculateSourceWindow(w, h, x, y, z, source_layer_id, &sw, &sh, &sx, &sy);

  return hasFrameForeground(sw, sh, sx, sy, source_layer_id);
}
bool FrameProvider::hasFrameForeground(int w, int h, int x, int y, int z) {
  //
  int n_videos = m_video_provider->getNumCamera();
  // cout << "n_videos:  " << n_videos << endl;
  Rect rect_window(x, y, w, h);

  for (int i = 0; i < n_videos; ++i) {
    Rect rect_video;
    assert(m_video_provider->getRect(rect_video, z, i));
    Rect rect_overlap = rect_video & rect_window;
    // cout << "rect_overlap: " << rect_overlap << endl;
    if (rect_overlap.width > 0) {
      return true;
    }
  }
  return false;
}
void FrameProvider::getFrameForeground(int w, int h, double x, double y,
                                       double z, cv::Mat &frame,
                                       cv::Mat &mask) {
  // calculate dst w, h, x, y, z
  int source_layer_id = getNearestLayer(z);
  int sx, sy, sw, sh;
  calculateSourceWindow(w, h, x, y, z, source_layer_id, &sw, &sh, &sx, &sy);

  vector<Rect> rect;
  getFrameForeground(sw, sh, sx, sy, source_layer_id, frame, mask, rect);

  double scale = (double)w / frame.cols;
  for (size_t i = 0; i < rect.size(); ++i) {
    rect[i].x *= scale;
    rect[i].y *= scale;
    rect[i].width *= scale;
    rect[i].height *= scale;
  }

  Size size(w, h);
  resize(frame, frame, size);
  resize(mask, mask, size);
  threshold(mask, mask, 250, 255, THRESH_BINARY);
  erode(mask, mask, Mat());

  // if video too small, show a rectangle around it
  if (z < 0.5) {
    for (size_t i = 0; i < rect.size(); ++i) {
      rectangle(frame, rect[i], Scalar(255, 0, 0), 2);
      rectangle(mask, rect[i], Scalar(255), 2);
    }
  }
}
void FrameProvider::getFrameForeground(int w, int h, int x, int y, int z,
                                       cv::Mat &frame, cv::Mat &mask,
                                       std::vector<cv::Rect> &rect) {
  int n_videos = m_video_provider->getNumCamera();

  frame = cv::Mat(h, w, CV_8UC3, Scalar(0, 0, 0));
  mask = cv::Mat(h, w, CV_8UC1, Scalar(0));

  rect.clear();
  Rect rect_window(x, y, w, h);
  for (int i = 0; i < n_videos; ++i) {
    Rect rect_video;
    assert(m_video_provider->getRect(rect_video, z, i));
    Rect rect_overlap = rect_video & rect_window;

    if (rect_overlap.width <= 0) {
      continue;
    }
    Mat video_frame, video_mask;
    assert(m_video_provider->getFrame(video_frame, video_mask, z, i));

    Rect rect_on_video = rect_overlap;
    rect_on_video.x -= rect_video.x;
    rect_on_video.y -= rect_video.y;

    Rect rect_on_win = rect_overlap;
    rect_on_win.x -= x;
    rect_on_win.y -= y;

    Mat video_frame_roi = video_frame(rect_on_video);
    Mat video_mask_roi = video_mask(rect_on_video);
    Mat frame_roi = frame(rect_on_win);
    Mat mask_roi = mask(rect_on_win);

    video_frame_roi.copyTo(frame_roi, video_mask_roi);
    video_mask_roi.copyTo(mask_roi, video_mask_roi);

    // imshow("frame_roi", frame_roi);
    // imshow("mask_roi", mask_roi);

    rect.push_back(rect_on_win); // add rect_on_win
  }                              // for video
}
bool FrameProvider::getThumbnail(cv::Mat &thumbnail, int camera_id) {
  //
  return m_video_provider->getThumbnail(thumbnail, camera_id);
}

void FrameProvider::copyMatToMat(Mat &src_mat, Rect &src_rect, Mat &dst_mat,
                                 Rect &dst_rect) {
  Rect src(0, 0, src_mat.cols, src_mat.rows);
  Rect dst(src_rect.x - dst_rect.x, src_rect.y - dst_rect.y, src_mat.cols,
           src_mat.rows);

  if (dst.x + dst.width > dst_mat.cols) {
    int shift = dst.x + dst.width - dst_mat.cols;
    src.width -= shift;
    dst.width -= shift;
  }
  if (dst.y + dst.height > dst_mat.rows) {
    src.height -= dst.y + dst.height - dst_mat.rows;
    dst.height = src.height;
  }
  if (dst.x < 0) {
    int shift = -dst.x;
    src.x += shift;
    src.width -= shift;
    dst.x += shift;
    dst.width -= shift;
  }
  if (dst.y < 0) {
    int shift = -dst.y;
    src.y += shift;
    src.height -= shift;
    dst.y += shift;
    dst.height -= shift;
  }
  src_mat(src).copyTo(dst_mat(dst));
}
int FrameProvider::getNumLayers() {
  // getNumLayers
  return m_tile_provider->getNumLayers();
}
int FrameProvider::getLayerWidth(int layer_id) {
  return m_tile_provider->getPixelColsOfLayer(layer_id);
}
int FrameProvider::getLayerHeight(int layer_id) {
  return m_tile_provider->getPixelRowsOfLayer(layer_id);
}

int FrameProvider::getNearestLayer(double z) {
  int source_layer_id = static_cast<int>(z + 1);
  source_layer_id = max(source_layer_id, 0);
  source_layer_id = min(source_layer_id, m_tile_provider->getNumLayers() - 1);
  return source_layer_id;
}
void FrameProvider::calculateSourceWindow(int w, int h, double x, double y,
                                          double z, int source_layer_id,
                                          int *sw, int *sh, int *sx, int *sy) {
  int pixel_x = static_cast<int>(
      x * m_tile_provider->getPixelColsOfLayer(source_layer_id));
  int pixel_y = static_cast<int>(
      y * m_tile_provider->getPixelRowsOfLayer(source_layer_id));

  double zoom = pow(2.0, (double)source_layer_id - z);
  int source_width = zoom * w;
  int source_height = zoom * h;

  pixel_x -= source_width / 2;
  pixel_y -= source_height / 2;

  if (sw)
    *sw = source_width;
  if (sw)
    *sh = source_height;
  if (sw)
    *sx = pixel_x;
  if (sw)
    *sy = pixel_y;
}