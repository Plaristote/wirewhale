#include "qabstractpacketsniffer.h"

QAbstractPacketSniffer::QAbstractPacketSniffer(const QString& interface_name, QObject *parent) :
    QObject(parent), interface_name(interface_name)
{
  max_captured_packets = 8;
}

QVector<QPacket> QAbstractPacketSniffer::pullPendingPackets(QVector<QPacket> list)
{
  pending_packets_mutex.lock();
  list = pending_packets.toVector();
  pending_packets.clear();
  pending_packets_mutex.unlock();
  return (list);
}
