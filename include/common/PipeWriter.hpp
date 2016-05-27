#ifndef PIPE_WRITER_HPP_
#define PIPE_WRITER_HPP_

#include <string>

class PipeWriter {
public:
  PipeWriter(std::string name);
  bool writeSomething(const void *buf, int nbyte);

private:
  int m_handle;
};

#endif