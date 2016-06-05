#include "WindowProviderLocal.hpp"

WindowProviderLocal::WindowProviderLocal(std::string path, int mode_video,
                                         int w, int h) {
  //   width = w;
  //   height = h;
  //   provider = make_shared<FrameProvider>(path, mode_video);

  //   int n_layers = provider->getNumLayers();
  //   Size top_layer_size(provider->getLayerWidth(n_layers - 1),
  //                       provider->getLayerHeight(n_layers - 1));
  //   Size winsize(w, h);
  //   controller = make_shared<WindowController>(n_layers, top_layer_size,
  //   winsize);
}

void WindowProviderLocal::setWindowPosition(double x, double y, double z) {
  //
}
bool WindowProviderLocal::hasFrame() {
  //
  return false;
}
void WindowProviderLocal::getFrame(cv::Mat &frame) {
  //
}
bool WindowProviderLocal::hasThumbnail() {
  //
  return false;
}
void WindowProviderLocal::getThumbnail(std::vector<cv::Mat> &thumbnail) {
  //
}