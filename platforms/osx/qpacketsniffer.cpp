#include "platforms/osx/qpacketsniffer.h"
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h> //For standard things
#include <stdlib.h>    //malloc
#include <string.h>    //strlen
#include <unistd.h>

#include <net/if.h>
#include <netinet/ip_icmp.h>   //Provides declarations for icmp header
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <netinet/if_ether.h>  //For ETH_P_ALL
#include <net/ethernet.h>  //For ether_header
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <iostream>

#include <QThread>

// http://www.lists.apple.com/archives/darwin-dev/2011/Jan/msg00049.html

using namespace std;

QPacketSniffer::QPacketSniffer(const QString& interface_name, QObject* parent) : QAbstractPacketSniffer(interface_name, parent)
{
  sock = ::socket(PF_NDRV, SOCK_RAW, 0);
  initialize_sock_address();
  if ((::bind(sock, (struct sockaddr*)&sock_address, sizeof(sock_address))) == -1)
    throw std::runtime_error("cannot bind socket for interface '" + interface_name.toStdString() + "': " + strerror(errno));
  initialize_sock_address();
}

QPacketSniffer::~QPacketSniffer()
{
  close(sock);
}

void QPacketSniffer::initialize_sock_address()
{
  int name_length = interface_name.length();

  strncpy((char*)sock_address.snd_name, interface_name.toStdString().c_str(), name_length > IFNAMSIZ ? IFNAMSIZ : name_length);
  sock_address.snd_name[name_length] = 0;
  sock_address.snd_len    = sizeof(sock_address);
  sock_address.snd_family = AF_NDRV;
}

void QPacketSniffer::initialize_protocol()
{
  struct ndrv_protocol_desc protocol_description;
  struct ndrv_demux_desc    demux_description[1];

  protocol_description.version         = (u_int32_t)1;
  protocol_description.protocol_family = (u_int32_t)13458;
  protocol_description.demux_count     = 1;
  protocol_description.demux_list      = (struct ndrv_demux_desc*)(&demux_description);
  demux_description[0].type            = NDRV_DEMUXTYPE_ETHERTYPE;
  demux_description[0].length          = 2;
  demux_description[0].data.ether_type = htons(0x0f0f);
  if ((setsockopt(sock, SOL_NDRVPROTO, NDRV_SETDMXSPEC, (caddr_t*)&protocol_description, sizeof(protocol_description))) == -1)
    throw std::runtime_error("cannot initialize protocol for interface '" + interface_name.toStdString() + "': " + strerror(errno));
}

void QPacketSniffer::run()
{
  mutex.lock();
  sniffing_thread = QThread::currentThreadId();
  must_stop = false;
  while (!must_stop)
    poll.run();
  mutex.unlock();
}

void QPacketSniffer::wait()
{
  if (sniffing_thread != QThread::currentThreadId())
  {
    mutex.lock();
    mutex.unlock();
  }
}

void QPacketSniffer::stop()
{
  must_stop = true;
}

QPacketSniffer::Poll::Poll()
{
  max_fd = 0;
}

QPacketSniffer::Poll::~Poll()
{
}

void QPacketSniffer::Poll::run()
{
  std::cout << "QPacketSniffer::Poll::run" << std::endl;
  int n;

  reset_fds();
  n = select(max_fd, &read_set, 0, 0, 0);
  if (n > 0)
  {
    foreach(int fd, read_set_fds)
    {
      if (FD_ISSET(fd, &read_set))
        on_event(fd);
    }
  }
  std::cout << "Done selecting" << std::endl;
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
