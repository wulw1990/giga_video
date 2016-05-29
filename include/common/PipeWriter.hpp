#ifndef PIPE_WRITER_HPP_
#define PIPE_WRITER_HPP_

#include <string>

class PipeWriter {
public:
  PipeWriter(std::string pipe_name);
  bool makePipe();
  bool openPipe();
  bool writeSomething(const void *buf, int nbyte);

private:
std::string m_pipe_name;
  int m_pipe_handle;
  bool m_pipe_open;
};

#endif