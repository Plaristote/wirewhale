#include "qpacketsniffer.h"

QPacketSniffer::QPacketSniffer(const QString& interface_name, QObject* parent) : QAbstractPacketSniffer(interface_name, parent)
{
  sock = ::socket(PF_PACKET, SOCK_RAW, htos(ETH_P_ALL));
  initialize_interface();
  initialize_sock_address();
  bind(sock, (struct sockaddr*)&sock_address, sizeof(sock_address));
}

QPacketSniffer::~QPacketSniffer()
{
  close(sock);
}

void QPacketSniffer::initialize_interface()
{
  memset(&interface, 0, sizeof(interface));
  ifr.ifr_name = interface_name;
  if ((ioctl(sock, SIOGIFINDEX, &interface)) == -1)
    throw std::runtime_error("cannot initialize interface " + interface_name.toStdString());
}

void QPacketSniffer::initialize_sock_address()
{
  memset(&sock_address, 0, sizeof(sock_address));
  sock_address.sll_family = AF_PACKET;
  sock_address.ifindex    = interface.ifr_ifindex;
  sock_address.protocol   = htons(ETH_P_ALL);
}

void QPacketSniffer::run()
{
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
