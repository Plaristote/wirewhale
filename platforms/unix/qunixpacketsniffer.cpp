#include "qunixpacketsniffer.h"
#include <unistd.h>

QUnixPacketSniffer::QUnixPacketSniffer(const QString& interface_name, QObject *parent) : QAbstractPacketSniffer(interface_name, parent)
{
}

void QUnixPacketSniffer::capture_packet()
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
      qpacket.length      = k;
      qpacket.time        = time(0);
      pending_packets << qpacket;
      if (is_first_received_packet)
        emit packetsReceived();
    }
    n_packet_read++;
  } while (k > 0 && n_packet_read < max_captured_packets);
  pending_packets_mutex.unlock();
}
