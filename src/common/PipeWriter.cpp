#include "PipeWriter.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>
using namespace std;

PipeWriter::PipeWriter(std::string pipe_name) {
  //
  m_pipe_name = pipe_name;
  m_pipe_open = false;
}
bool PipeWriter::makePipe() {
  int res = mkfifo(m_pipe_name.c_str(), 0777);
  if (res != 0) {
    fprintf(stderr, "Could not create fifo %s\n", m_pipe_name.c_str());
    return false;
  }
  cout << "makePipe ok" << endl;
  return true;
}
bool PipeWriter::openPipe() {
  cout << "openPipe begin" << endl;
  m_pipe_handle = open(m_pipe_name.c_str(), O_WRONLY);
  cout << "m_pipe_handle: " << m_pipe_handle << endl;
  if (m_pipe_handle == -1) {
    fprintf(stderr, "Could not open fifo %s\n", m_pipe_name.c_str());
    return false;
  }
  cout << "openPipe ok" << endl;
  m_pipe_open = true;
  return true;
}
bool PipeWriter::writeSomething(const void *buf, int nbyte) {
  //
  if (!m_pipe_open) {
    cout << "PipeWriter::writeSomething: pipe not opened" << endl;
    return false;
  }
  if (write(m_pipe_handle, buf, nbyte) != nbyte) {
    cout << "PipeWriter::writeSomething: pipe write error" << endl;
    return false;
  }
  return true;
}