#ifndef QPACKETTABLE_H
#define QPACKETTABLE_H

#include <QAbstractTableModel>
#include "qpacket.h"
#include <QList>

class QPacketTable : public QAbstractTableModel
{
  Q_OBJECT
public:
  enum Roles
  {
    RoleNumber = Qt::UserRole + 1,
    RoleAlert,
    RoleTime,
    RoleSource,
    RoleDestination,
    RoleProtocol,
    RoleLength,
    RoleInformation,
    RolePayload
  };

  explicit QPacketTable(QObject *parent = 0);

  QHash<int, QByteArray> roleNames  ()                                   const Q_DECL_OVERRIDE;
  int                    rowCount   (const QModelIndex& parent)          const Q_DECL_OVERRIDE;
  int                    columnCount(const QModelIndex& parent)          const Q_DECL_OVERRIDE;
  QVariant               data       (const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
  Qt::ItemFlags          flags      (const QModelIndex& index)           const Q_DECL_OVERRIDE { return (Qt::ItemIsEditable); }

  void     addPackets(QVector<QPacket> packets);
  QPacket  packet(long number);

signals:
  void     packetsAdded(QVector<QPacket>);

public slots:
  void     clear();
  QVariant getDataAt(int x, QString role);

protected:
  void     addPacket(QPacket);

private:
  QList<QPacket> packets;
};

#endif // QPACKETTABLE_H
