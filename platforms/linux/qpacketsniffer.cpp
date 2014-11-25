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

QPacketSniffer::Interface::Interface(const QString &name)
{
  int name_length = name.length() > IFNAMSIZ - 1 ? IFNAMSIZ - 1 : name.length();

  memset(&data, 0, sizeof(data));
  strncpy(data.ifr_name, name.toUtf8().data(), name_length);
}

QPacketSniffer::Interface::~Interface()
{
}

void QPacketSniffer::Interface::initialize_if_index(int fd)
{
  if ((ioctl(fd, SIOGIFINDEX, &data)) == -1)
    raise("cannot initialize interface");
}

void QPacketSniffer::Interface::enable_promiscuous_mode(int fd)
{
  if (!(data.ifr_flags & IFF_PROMISC))
  {
    data.ifr_flags |= IFF_PROMISC;
    if ((ioctl(fd, SIOCSIFFLAGS, &data)) == -1)
      raise("cannot enable promiscuous mode");
  }
}

void QPacketSniffer::Interface::disable_promiscuous_mode(int fd)
{
  if (data.ifr_flags & IFF_PROMISC)
  {
    data.ifr_flags &= ~IFF_PROMISC;
    if ((ioctl(fd, SIOCSIFFLAGS, &data)) == -1)
      raise("cannot disable promiscuous mode");
  }
}

void QPacketSniffer::Interface::raise(const QString &message)
{
    QString full_message = message + " for interface '" + QString(data.ifr_name) + "': " + QString(strerror(errno));

    throw std::runtime_error(full_message.toUtf8().data());
}

QPacketSniffer::QPacketSniffer(const QString& interface_name, QObject* parent) :
    QAbstractPacketSniffer(interface_name, parent),
    interface(interface_name)
{
  sock      = ::socket(PF_PACKET, SOCK_RAW, get_protocol());
  initialize_interface();
  initialize_sock_address();
  bind(sock, (struct sockaddr*)&sock_address, sizeof(sock_address));
  poll.on_event  = [this](int) { capture_packet(); };
  poll.watch(sock);
}

QPacketSniffer::~QPacketSniffer()
{
  interface.disable_promiscuous_mode(sock);
  close(sock);
}

void QPacketSniffer::initialize_interface()
{
  interface.initialize_if_index(sock);
  interface.enable_promiscuous_mode(sock);
}

void QPacketSniffer::initialize_sock_address()
{
  memset(&sock_address, 0, sizeof(sock_address));
  sock_address.sll_family   = AF_PACKET;
  sock_address.sll_ifindex  = interface.data.ifr_ifindex;
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

void QPacketSniffer::capture_packet()
{
  std::cout << "received packets" << std::endl;
  /*std::cout << "CAPTURED PACKET" << std::endl;
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
  std::cout << "DONE WITH PACKET CAPTURE" << std::endl;*/
}

size_t QPacketSniffer::packet_offset_ip_header()
{
  return sizeof(ether_header);
}

size_t QPacketSniffer::packet_offset_xcp_header()
{
  return packet_offset_ip_header() + sizeof(struct iphdr);
}
