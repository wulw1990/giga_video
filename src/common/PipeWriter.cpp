#include "PipeWriter.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>

using namespace std;

PipeWriter::PipeWriter(std::string name) {
  //
  int res = mkfifo(name.c_str(), 0777);
  if (res != 0) {
    fprintf(stderr, "Could not create fifo %s\n", name.c_str());
    exit(EXIT_FAILURE);
  }

  m_handle = open(name.c_str(), O_RDONLY);
  if (m_handle != 0) {
    fprintf(stderr, "Could not open fifo %s\n", name.c_str());
    exit(EXIT_FAILURE);
  }
}
bool PipeWriter::writeSomething(const void *buf, int nbyte) {
  //
  if (write(m_handle, buf, nbyte) != nbyte) {
    return false;
  }
  return true;
}