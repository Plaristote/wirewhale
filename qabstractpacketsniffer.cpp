#include "qabstractpacketsniffer.h"

QAbstractPacketSniffer::QAbstractPacketSniffer(const QString& interface_name, QObject *parent) :
    QObject(parent), interface_name(interface_name)
{
  max_captured_packets = 8;
  captured_packets     = 0;
}

QVector<QPacket> QAbstractPacketSniffer::pullPendingPackets(QVector<QPacket> list)
{
  pending_packets_mutex.lock();
  list = pending_packets.toVector();
  pending_packets.clear();
  pending_packets_mutex.unlock();
  return (list);
}

bool QAbstractPacketSniffer::runFilters(const Packet& packet) const
{
  foreach (auto filter, filters)
  {
    if (filter(packet) == true)
      return true;
  }
  return false;
}
