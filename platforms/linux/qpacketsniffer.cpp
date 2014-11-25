#include "qpacketsniffer.h"
#include <unistd.h>
#include <linux/sockios.h>
#include <linux/if_ether.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stropts.h>
#include <string.h>
#include "qpacket.h"
#include <iostream>
#include <errno.h>

QPacketSniffer::QPacketSniffer(const QString& interface_name, QObject* parent) : QAbstractPacketSniffer(interface_name, parent)
{
  sock      = ::socket(PF_PACKET, SOCK_RAW, get_protocol());
  initialize_interface();
  initialize_sock_address();
  bind(sock, (struct sockaddr*)&sock_address, sizeof(sock_address));
  poll.on_event  = [this](int) { capture_packet(); };
}

QPacketSniffer::~QPacketSniffer()
{
  close(sock);
}

void QPacketSniffer::initialize_interface()
{
  std::cout << "QPacketSniffer(" << interface_name.toStdString() << ")" << std::endl;
  int name_length = interface_name.length() > IFNAMSIZ - 1 ? IFNAMSIZ - 1 : interface_name.length();

  memset(&interface, 0, sizeof(interface));
  strncpy(interface.ifr_name, interface_name.toUtf8().data(), name_length);
  if ((ioctl(sock, SIOGIFINDEX, &interface)) == -1)
    throw std::runtime_error("cannot initialize interface " + std::string(interface.ifr_name) + ": " + std::string(strerror(errno)));
}

void QPacketSniffer::initialize_sock_address()
{
  memset(&sock_address, 0, sizeof(sock_address));
  sock_address.sll_family   = AF_PACKET;
  sock_address.sll_ifindex  = interface.ifr_ifindex;
  sock_address.sll_protocol = get_protocol();
}

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
  std::cout << "polling" << std::endl;
  int n_events = epoll_wait(efd, events, max_events, 1000);

  for (int i = 0 ; i < n_events ; ++i)
    on_event(events[i].data.fd);
}

uint16_t QPacketSniffer::get_protocol() const
{
  return (htons(ETH_P_ALL));
}

void QPacketSniffer::run()
{
  mutex.lock();
  sniffing_thread = QThread::currentThreadId();
  must_stop       = false;
  while (!must_stop)
    poll.run();
  mutex.unlock();
}

void QPacketSniffer::stop()
{
  must_stop = true;
  if (sniffing_thread != QThread::currentThreadId())
  {
    mutex.lock(); // Joining thread
    mutex.unlock();
  }
}

void QPacketSniffer::capture_packet()
{
  int     packetSize = 65535;
  char    packet[packetSize];

  struct ether_header* eth = (struct ether_header*)packet;
  struct iphdr*        ip  = (struct iphdr*)(packet + packet_offset_ip_header());
  //struct tcphdr*       tcp = (struct tcphdr*)(packet + packet_offset_xcp_header());

  QPacket qpacket;
  int k;
  do
  {
    k = read(sock, packet, sizeof(packet));
    if (k > 0 && ntohs(eth->ether_type) == 0x0800)
    {
      char ip_string[16];

      inet_ntop(AF_INET, &ip->saddr, ip_string, 16);
      qpacket.source      = ip_string;
      inet_ntop(AF_INET, &ip->daddr, ip_string, 16);
      qpacket.destination = ip_string;

      switch (ip->protocol)
      {
      default:
        break ;
      }
    }
  } while (k);
}

size_t QPacketSniffer::packet_offset_ip_header()
{
  return sizeof(ether_header);
}

size_t QPacketSniffer::packet_offset_xcp_header()
{
  return packet_offset_ip_header() + sizeof(struct iphdr);
}
