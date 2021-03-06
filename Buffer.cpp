#include "Buffer.h"
//#include <bits/uio.h>
#include <sys/uio.h>

const size_t Buffer::INITSIZE = 1024;
const size_t Buffer::PREPEND = sizeof(unsigned long);//在64位电脑中为8bytes

size_t Buffer::readByFd(int fd)
{
  char buffer[65563];
  struct iovec vec[2];

  size_t space = writableBytes();
  vec[0].iov_base = begin() + writeIndex_;
  vec[0].iov_len = space;

  vec[1].iov_base = buffer;
  vec[1].iov_len = sizeof(buffer);

  int iovcnt = (space > sizeof(buffer)) ? 1 : 2;
  ssize_t n = ::readv(fd, vec, iovcnt);

  if(n < 0)
  {
    return n;
  }

  else if(static_cast<size_t>(n) < space)
  {
    writeIndex_ += n;
  }
  else
  {
    writeIndex_ = buffer_.size();
    append(buffer, n - space);
  }

  return n;
}
