#include "VirtualCameraDevice.hpp"

#include <opencv2/opencv.hpp>
using namespace cv;

/*
 * How to test v4l2loopback:
 * 1. launch this test program (even in background), it will initialize the
 *    loopback device and keep it open so it won't loose the settings.
 * 2. Feed the video device with data according to the settings specified
 *    below: size, pixelformat, etc.
 *    For instance, you can try the default settings with this command:
 *    mencoder video.avi -ovc raw -nosound -vf scale=640:480,format=yuy2 -o
 * /dev/video1
 *    TODO: a command that limits the fps would be better :)
 *
 * Test the video in your favourite viewer, for instance:
 *   luvcview -d /dev/video1 -f yuyv
 */

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
#define CHECK_REREAD
#endif

#define VIDEO_DEVICE "/dev/video0"
#if 1
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#else
#define FRAME_WIDTH 512
#define FRAME_HEIGHT 512
#endif

#if 0
#define FRAME_FORMAT V4L2_PIX_FMT_YUYV
#else
#define FRAME_FORMAT V4L2_PIX_FMT_YVU420
#endif

static int debug = 1;

int format_properties(const unsigned int format, const unsigned int width,
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

void print_format(struct v4l2_format *vid_format) {
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

static void RGB_to_YUV420(const unsigned char *rgb, unsigned char *yuv420,
                          int width, int height);

int VirtualCameraDevice::test(int argc, char **argv) {
  struct v4l2_capability vid_caps;
  struct v4l2_format vid_format;

  size_t framesize;
  size_t linewidth;

  __u8 *buffer;
  __u8 *check_buffer;

  const char *video_device = VIDEO_DEVICE;
  int fdwr = 0;
  int ret_code = 0;

  int i;

  if (argc > 1) {
    video_device = argv[1];
    printf("using output device: %s\n", video_device);
  }

  fdwr = open(video_device, O_RDWR);
  assert(fdwr >= 0);

  ret_code = ioctl(fdwr, VIDIOC_QUERYCAP, &vid_caps);
  assert(ret_code != -1);

  memset(&vid_format, 0, sizeof(vid_format));

  ret_code = ioctl(fdwr, VIDIOC_G_FMT, &vid_format);
  if (debug)
    print_format(&vid_format);

  vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
  vid_format.fmt.pix.width = FRAME_WIDTH;
  vid_format.fmt.pix.height = FRAME_HEIGHT;
  vid_format.fmt.pix.pixelformat = FRAME_FORMAT;
  vid_format.fmt.pix.sizeimage = framesize;
  vid_format.fmt.pix.field = V4L2_FIELD_NONE;
  vid_format.fmt.pix.bytesperline = linewidth;
  vid_format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

  if (debug)
    print_format(&vid_format);
  ret_code = ioctl(fdwr, VIDIOC_S_FMT, &vid_format);

  assert(ret_code != -1);

  if (debug)
    printf("frame: format=%d\tsize=%d\n", FRAME_FORMAT, framesize);

  print_format(&vid_format);

  if (!format_properties(vid_format.fmt.pix.pixelformat,
                         vid_format.fmt.pix.width, vid_format.fmt.pix.height,
                         &linewidth, &framesize)) {
    printf("unable to guess correct settings for format '%d'\n", FRAME_FORMAT);
  } else {
    printf("Success to guess correct settings for format '%d'\n", FRAME_FORMAT);
  }
  buffer = (__u8 *)malloc(sizeof(__u8) * framesize);
  check_buffer = (__u8 *)malloc(sizeof(__u8) * framesize);

  memset(buffer, 0, framesize);
  memset(check_buffer, 0, framesize);
  for (i = 0; i < framesize; ++i) {
    // buffer[i] = i % 2;
    check_buffer[i] = 0;
  }

  printf("buffer ok\n");

  string name_input = "../MVI_7305.avi";
  VideoCapture capture(name_input);
  assert(capture.isOpened());
  Size size_output(vid_format.fmt.pix.width, vid_format.fmt.pix.height);
  Mat frame;
  while (1) {
    capture.read(frame);
    if (frame.empty()) {
      capture.release();
      capture.open(name_input);
      assert(capture.isOpened());
      capture.read(frame);
      assert(!frame.empty());
    }
    cv::resize(frame, frame, size_output);
    imshow("frame", frame);
    waitKey(33);

    cvtColor(frame, frame, CV_BGR2RGB);
    RGB_to_YUV420(frame.data, buffer, vid_format.fmt.pix.width,
                  vid_format.fmt.pix.height);
    write(fdwr, buffer, framesize);
  }

  printf("write ok\n");

#ifdef CHECK_REREAD
  do {
    /* check if we get the same data on output */
    int fdr = open(video_device, O_RDONLY);
    read(fdr, check_buffer, framesize);
    for (i = 0; i < framesize; ++i) {
      assert(buffer[i] == check_buffer[i]);
    }
    close(fdr);
  } while (0);
#endif

  printf("ok\n");
  pause();
  printf("pause ok\n");

  close(fdwr);

  free(buffer);
  free(check_buffer);

  return 0;
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