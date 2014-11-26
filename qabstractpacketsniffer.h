#ifndef QABSTRACTPACKETSNIFFER_H
#define QABSTRACTPACKETSNIFFER_H

#include <QObject>
#include <QMutex>
#include <QVector>
#include <QList>
#include "qpacket.h"

class QAbstractPacketSniffer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString interface_name READ getInterfaceName)
public:
    explicit QAbstractPacketSniffer(const QString& interface_name, QObject *parent = 0);

    const QString& getInterfaceName(void) { return (interface_name); }

    virtual void run(void)  = 0;
    virtual void wait(void) = 0;
    virtual void stop(void) = 0;

    QVector<QPacket> pullPendingPackets(QVector<QPacket> list);
signals:
    void packetsReceived();

public slots:

protected:
    QString        interface_name;
    QList<QPacket> pending_packets;
    QMutex         pending_packets_mutex;
    short          max_captured_packets;
};

#endif // QABSTRACTPACKETSNIFFER_H
