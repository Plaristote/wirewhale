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

struct Packet
{
  Packet()
  {
    memset(buffer, 0, sizeof(buffer));
    eth = (struct ether_header*)buffer;
    ip  = (struct iphdr*)(buffer + QPacketSniffer::packet_offset_ip_header());
    //struct tcphdr*       tcp = (struct tcphdr*)(packet + packet_offset_xcp_header());
  }

  enum EtherType
  {
    ETHER_TYPE_UNKNOWN = 0,
    IPv4 = 0x0800,
    IPv6 = 0x86DD,
    ARP  = 0x0806
  };

  bool has_supported_type()
  {
    switch (get_ether_type())
    {
      case IPv4:
      case IPv6:
      case ARP:
          return (true);
    }
    return (false);
  }

  EtherType get_ether_type(void) const
  {
    return (EtherType)(ntohs(eth->ether_type));
  }

  QString get_source_ip(void) const
  {
    switch (get_ether_type())
    {
    case IPv4:
      return (get_source_ipv4());
    case IPv6:
      return (get_source_ipv6());
    default:
      break ;
    }
    return ("");
  }

  QString get_destination_ip(void) const
  {
    switch (get_ether_type())
    {
    case IPv4:
      return (get_destination_ipv4());
    case IPv6:
      return (get_destination_ipv6());
    default:
      break ;
    }
    return "";
  }

  QString get_protocol(void) const
  {
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
      return "tcp";
    case IPPROTO_UDP:
      return "udp";
    case IPPROTO_ICMP:
      return "icmp";
    case IPPROTO_IGMP:
      return "igmp";
    case IPPROTO_RSVP:
      return "rsvp";
    case IPPROTO_SCTP:
      return "sctp";
    case IPPROTO_DCCP:
      return "dccp";
    case IPPROTO_ESP:
      return "esp";
    case IPPROTO_AH:
      return "ah";
    default:
      break ;
    }
    return "?";
  }

  char                 buffer[65535];
  struct ether_header* eth;
  struct iphdr*        ip;

private:
  QString get_source_ipv6(void) const
  {
      return ("");
  }

  QString get_destination_ipv6(void) const
  {
      return ("");
  }

  QString get_source_ipv4(void) const
  {
    char ip_string[16];

    inet_ntop(AF_INET, &ip->saddr, ip_string, 16);
    return (ip_string);
  }

  QString get_destination_ipv4(void) const
  {
    char ip_string[16];

    inet_ntop(AF_INET, &ip->daddr, ip_string, 16);
    return (ip_string);
  }
};

void QPacketSniffer::capture_packet()
{
  Packet  packet;
  int     k;
  int     n_packet_read = 0;

  pending_packets_mutex.lock();
  do
  {
    k = read(sock, packet.buffer, sizeof(packet.buffer));
    if (packet.has_supported_type())
    {
      QPacket qpacket;
      bool    is_first_received_packet = pending_packets.count() == 0;

      switch (packet.get_ether_type())
      {
      case Packet::IPv4:
          qpacket.ethernet_type = QPacket::IPv4;
      case Packet::IPv6:
          qpacket.ethernet_type = QPacket::IPv6;
      case Packet::ARP:
          qpacket.ethernet_type = QPacket::ARP;
      }
      qpacket.source      = packet.get_source_ip();
      qpacket.destination = packet.get_destination_ip();
      qpacket.protocol    = packet.get_protocol();
      pending_packets << qpacket;
      if (is_first_received_packet)
        emit packetsReceived();
    }
    n_packet_read++;
  } while (k > 0 && n_packet_read < max_captured_packets);
  pending_packets_mutex.unlock();
}

size_t QPacketSniffer::packet_offset_ip_header()
{
  return sizeof(ether_header);
}

size_t QPacketSniffer::packet_offset_xcp_header()
{
  return packet_offset_ip_header() + sizeof(struct iphdr);
}
