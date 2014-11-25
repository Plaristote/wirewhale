#include "qpacketsniffer.h"

QPacketSniffer::QPacketSniffer(const QString& interface_name, QObject* parent) : QAbstractPacketSniffer(parent), interface_name(interface_name)
{
  sock == ::socket(PF_PACKET, SOCK_RAW, htos(ETH_P_ALL));
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
