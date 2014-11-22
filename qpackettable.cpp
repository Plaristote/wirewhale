#include "qpackettable.h"

#define COLUMN_COUNT 7

QPacketTable::QPacketTable(QObject *parent) :
  QAbstractTableModel(parent)
{
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
  return roles;
}

void QPacketTable::addPackets(QVector<QPacket> packets)
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
      return packet.number;
    case RoleTime:
      return packet.time;
    case RoleSource:
      return packet.source;
    case RoleDestination:
      return packet.destination;
    case RoleProtocol:
      return packet.protocol;
    case RoleLength:
      return packet.length;
    case RoleInformation:
      return packet.information;
  }
  return (1);
}
