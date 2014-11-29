#include "qpackettable.h"
#include <QVariant>
#include <iostream>

#define COLUMN_COUNT 7

QPacketTable::QPacketTable(QObject *parent) :
  QAbstractTableModel(parent)
{
  max_packets = 10;
}

QHash<int, QByteArray> QPacketTable::roleNames() const
{
  QHash<int, QByteArray> roles;

  roles[RoleAlert]       = "alert";
  roles[RoleNumber]      = "number";
  roles[RoleTime]        = "time";
  roles[RoleSource]      = "source";
  roles[RoleDestination] = "destination";
  roles[RoleProtocol]    = "protocol";
  roles[RoleLength]      = "length";
  roles[RoleInformation] = "information";
  roles[RolePayload]     = "payload";
  return roles;
}

void QPacketTable::addPackets(QVector<QPacket> packets)
{
  while (packets.count() > max_packets)
    packets.removeFirst();
  if (packets.count() + this->packets.count() > max_packets)
  {
    int to_remove = (packets.count() + this->packets.count()) - max_packets;

    if (to_remove > this->packets.count())
      to_remove = this->packets.count();
    beginRemoveRows(QModelIndex(), 0, to_remove);
    while (--to_remove >= 0)
      this->packets.removeFirst();
    endRemoveRows();
  }
  {
    int firstRow = this->packets.count();

    beginInsertRows(QModelIndex(), firstRow, firstRow + packets.count() - 1);
    foreach (QPacket packet, packets)
      addPacket(packet);
    endInsertRows();
    emit layoutChanged();
    emit dataChanged(index(firstRow, 0), index(firstRow + packets.count() - 1, COLUMN_COUNT));
    emit packetsAdded(packets);
  }
}

void QPacketTable::addPacket(QPacket packet)
{
  packets.push_back(packet);
}

QPacket QPacketTable::packet(long number)
{
  auto it = std::find(packets.begin(), packets.end(), number);

  if (it == packets.end())
    return (QPacket());
  return (*it);
}

void QPacketTable::clear()
{
  int count = packets.count();

  if (count > 0)
  {
    beginRemoveRows(QModelIndex(), 0, count);
    packets.clear();
    endRemoveRows();
    emit layoutChanged();
    emit dataChanged(index(0, 0), index(count, COLUMN_COUNT));
  }
}

QVariant QPacketTable::getDataAt(int row, QString role)
{
  if (row < 0 || packets.size() <= row)
    return "";
  return data(createIndex(row, 0), roleNames().key(QByteArray(role.toStdString().c_str())));
}

int QPacketTable::rowCount(const QModelIndex&) const
{
  return (packets.size());
}

int QPacketTable::columnCount(const QModelIndex&) const
{
  return (COLUMN_COUNT);
}

QVariant QPacketTable::data(const QModelIndex& index, int role) const
{
  QPacket packet = packets.at(index.row());
  switch (role)
  {
    case RoleAlert:
      return 0;
    case RoleNumber:
      return QString::number(packet.number);
    case RoleTime:
      return packet.dateTime().toString("h:mm:ss dd/MM/yy");
    case RoleSource:
      return packet.source;
    case RoleDestination:
      return packet.destination;
    case RoleProtocol:
      return packet.protocol;
    case RoleLength:
      return QString::number(packet.length);
    case RoleInformation:
      return packet.information;
  case RolePayload:
      return packet.payload;
  }
  return (1);
}
