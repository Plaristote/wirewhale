#include "qpacketsniffer.h"
#include <unistd.h>

QPacketSniffer::Poll::Poll()
{
  efd = epoll_create1(0);
  if (efd == -1)
    throw std::runtime_error("cannot initialize epoll");
  max_events = 128;
  events     = new struct epoll_event[max_events];
}

QPacketSniffer::Poll::~Poll()
{
  close(efd);
  delete[] events;
}

void QPacketSniffer::Poll::watch(int fd)
{
  struct epoll_event event;

  event.data.fd = fd;
  event.events  = EPOLLIN | EPOLLET;
  epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event);
}

void QPacketSniffer::Poll::run()
{
  int n_events = epoll_wait(efd, events, max_events, 1000);

  for (int i = 0 ; i < n_events ; ++i)
    on_event(events[i].data.fd);
}
