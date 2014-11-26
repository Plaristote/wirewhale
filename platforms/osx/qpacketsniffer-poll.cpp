#include "qpacketsniffer.h"

QPacketSniffer::Poll::Poll()
{
  max_fd = 0;
}

QPacketSniffer::Poll::~Poll()
{
}

void QPacketSniffer::Poll::run()
{
  int n;

  timeout.tv_sec  = 1;
  timeout.tv_usec = 0;
  reset_fds();
  n = select(max_fd, &read_set, 0, 0, &timeout);
  if (n > 0)
  {
    foreach(int fd, read_set_fds)
    {
      if (FD_ISSET(fd, &read_set))
        on_event(fd);
    }
  }
}

void QPacketSniffer::Poll::reset_fds()
{
  FD_ZERO(&read_set);
  foreach(int fd, read_set_fds)
    FD_SET(fd, &read_set);
}

void QPacketSniffer::Poll::watch(int fd)
{
  read_set_fds << fd;
  if (fd > max_fd)
    max_fd = fd;
}
