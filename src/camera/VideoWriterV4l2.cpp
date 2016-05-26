#include "VideoWriterV4l2.hpp"

#include <iostream>

using namespace std;
using namespace cv;

#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define ROUND_UP_2(num) (((num) + 1) & ~1)
#define ROUND_UP_4(num) (((num) + 3) & ~3)
#define ROUND_UP_8(num) (((num) + 7) & ~7)
#define ROUND_UP_16(num) (((num) + 15) & ~15)
#define ROUND_UP_32(num) (((num) + 31) & ~31)
#define ROUND_UP_64(num) (((num) + 63) & ~63)

#if 0
#define FRAME_FORMAT V4L2_PIX_FMT_YUYV
#else
#define FRAME_FORMAT V4L2_PIX_FMT_YVU420
#endif
static int debug = 1;

static int format_properties(const unsigned int format,
                             const unsigned int width,
                             const unsigned int height, size_t *linewidth,
                             size_t *framewidth) {
  size_t lw, fw;
  switch (format) {
  case V4L2_PIX_FMT_YUV420:
  case V4L2_PIX_FMT_YVU420:
    lw = width; /* ??? */
    fw = ROUND_UP_4(width) * ROUND_UP_2(height);
    fw += 2 * ((ROUND_UP_8(width) / 2) * (ROUND_UP_2(height) / 2));
    break;
  case V4L2_PIX_FMT_UYVY:
  case V4L2_PIX_FMT_Y41P:
  case V4L2_PIX_FMT_YUYV:
  case V4L2_PIX_FMT_YVYU:
    lw = (ROUND_UP_2(width) * 2);
    fw = lw * height;
    break;
  default:
    return 0;
  }

  if (linewidth)
    *linewidth = lw;
  if (framewidth)
    *framewidth = fw;

  return 1;
}
static void print_format(struct v4l2_format *vid_format) {
  printf("----print_format----\n");
  printf("	vid_format->type                =%d\n", vid_format->type);
  printf("	vid_format->fmt.pix.width       =%d\n",
         vid_format->fmt.pix.width);
  printf("	vid_format->fmt.pix.height      =%d\n",
         vid_format->fmt.pix.height);
  printf("	vid_format->fmt.pix.pixelformat =%d\n",
         vid_format->fmt.pix.pixelformat);
  printf("	vid_format->fmt.pix.sizeimage   =%d\n",
         vid_format->fmt.pix.sizeimage);
  printf("	vid_format->fmt.pix.field       =%d\n",
         vid_format->fmt.pix.field);
  printf("	vid_format->fmt.pix.bytesperline=%d\n",
         vid_format->fmt.pix.bytesperline);
  printf("	vid_format->fmt.pix.colorspace  =%d\n",
         vid_format->fmt.pix.colorspace);
}

VideoWriterV4l2::VideoWriterV4l2(std::string device_path,
                                 cv::Size output_size) {
  cout << "device_path: " << device_path << endl;
  cout << "output_size: " << output_size << endl;

  m_output_size = output_size;

  m_fdwr = 0;
  m_framesize = 0;
  {
    struct v4l2_capability vid_caps;
    struct v4l2_format vid_format;
    size_t linewidth = 0;
    int ret_code = 0;
    m_fdwr = open(device_path.c_str(), O_RDWR);
    assert(m_fdwr >= 0);

    ret_code = ioctl(m_fdwr, VIDIOC_QUERYCAP, &vid_caps);
    assert(ret_code != -1);

    memset(&vid_format, 0, sizeof(vid_format));

    ret_code = ioctl(m_fdwr, VIDIOC_G_FMT, &vid_format);
    if (debug)
      print_format(&vid_format);

    vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    vid_format.fmt.pix.width = m_output_size.width;
    vid_format.fmt.pix.height = m_output_size.height;
    vid_format.fmt.pix.pixelformat = FRAME_FORMAT;
    vid_format.fmt.pix.sizeimage = m_framesize;
    vid_format.fmt.pix.field = V4L2_FIELD_NONE;
    vid_format.fmt.pix.bytesperline = linewidth;
    vid_format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

    if (debug)
      print_format(&vid_format);
    ret_code = ioctl(m_fdwr, VIDIOC_S_FMT, &vid_format);

    assert(ret_code != -1);

    if (debug)
      cout << "frame: format= " << FRAME_FORMAT << "\tsize=" << m_framesize
           << endl;

    if (debug)
      print_format(&vid_format);

    if (!format_properties(vid_format.fmt.pix.pixelformat,
                           vid_format.fmt.pix.width, vid_format.fmt.pix.height,
                           &linewidth, &m_framesize)) {
      printf("unable to guess correct settings for format '%d'\n",
             FRAME_FORMAT);
    }

    m_output_size.width = vid_format.fmt.pix.width;
    m_output_size.height = vid_format.fmt.pix.height;
  }
  cout << "framesize: " << m_framesize << endl;

  m_buffer = (__u8 *)malloc(sizeof(__u8) * m_framesize);
  cout << "framesize: " << m_framesize << endl;
}
static void RGB_to_YUV420(const unsigned char *rgb, unsigned char *yuv420,
                          int width, int height);
void VideoWriterV4l2::writeFrame(const cv::Mat frame_) {
  //
  Mat frame = frame_.clone();
  cv::resize(frame, frame, m_output_size);
  cvtColor(frame, frame, CV_BGR2RGB);
  RGB_to_YUV420(frame.data, m_buffer, m_output_size.width,
                m_output_size.height);
  if (write(m_fdwr, m_buffer, m_framesize) != (int)m_framesize) {
    cout << "write error" << endl;
    exit(-1);
  }
}
void VideoWriterV4l2::release() {
  //
  cout << "release" << endl;
}

static void rgb_to_yuv(unsigned char b, unsigned char g, unsigned char r,
                       unsigned char &y, unsigned char &u, unsigned char &v) {
  float yf, uf, vf;

  // Y = R * 0.299 + G * 0.587 + B * 0.114;
  // U = R * -0.169 + G * -0.332 + B * 0.500 + 128.0;
  // V = R * 0.500 + G * -0.419 + B * -0.0813 + 128.0;

  yf = 0.299f * static_cast<float>(r) + 0.587f * static_cast<float>(g) +
       0.114f * static_cast<float>(b);
  yf = (yf > 255.0f) ? 255.0f : yf;
  yf = (yf < 0.0f) ? 0.0f : yf;
  y = static_cast<unsigned char>(yf);

  uf = -0.169f * static_cast<float>(r) - 0.332f * static_cast<float>(g) +
       0.500f * static_cast<float>(b) + 128.0;
  uf = (uf > 255.0f) ? 255.0f : uf;
  uf = (uf < 0.0f) ? 0.0f : uf;
  u = static_cast<unsigned char>(uf);

  vf = 0.500f * static_cast<float>(r) - 0.419f * static_cast<float>(g) -
       0.081f * static_cast<float>(b) + 128.0;
  vf = (vf > 255.0f) ? 255.0f : vf;
  vf = (vf < 0.0f) ? 0.0f : vf;
  v = static_cast<unsigned char>(vf);
}

static void RGB_to_YUV420(const unsigned char *rgb, unsigned char *yuv420,
                          int width, int height) {
  unsigned char *y_pixel = yuv420;
  unsigned char *u_pixel = yuv420 + width * height;
  unsigned char *v_pixel = yuv420 + width * height + (width * height / 4);

  unsigned char *U_tmp = new unsigned char[width * height];
  unsigned char *V_tmp = new unsigned char[width * height];

  int index = 0;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      rgb_to_yuv(rgb[3 * (y * width + x) + 0], rgb[3 * (y * width + x) + 1],
                 rgb[3 * (y * width + x) + 2], y_pixel[index], U_tmp[index],
                 V_tmp[index]);
      index++;
    }
  }

  index = 0;
  for (int y = 0; y < height; y += 2) {
    for (int x = 0; x < width; x += 2) {
      u_pixel[index] = U_tmp[y * width + x];
      v_pixel[index] = V_tmp[y * width + x];
      index++;
    }
  }

  delete[] U_tmp;
  delete[] V_tmp;
}