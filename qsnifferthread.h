#ifndef QSNIFFERTHREAD_H
#define QSNIFFERTHREAD_H

#include <QThread>
#include <QMutex>
#include "qpacket.h"
#include <QList>
#include <QVector>
#include "qpacketsniffer.h"
#include <QString>

class QSnifferThread : public QThread
{
    Q_OBJECT
public:
    explicit QSnifferThread(QObject *parent = 0);

    QVector<QPacket> receivedPackets();
    bool             isSniffing();

signals:
  void packetReceived();
  void snifferFailedToStart(QString);

public slots:
  void start();
  void startSniffing();
  void stopSniffing();
  void changeInterface(QString interface_);

private:
  void            run();
  void            initializeSniffer();
  void            deleteSniffer();

  QString         interface_;
  QMutex          mutex;
  bool            mustStop;
  QPacketSniffer* sniffer;
  long            number;

  QMutex          list_mutex;
  QList<QPacket>  received_packets;
};

#endif // QSNIFFERTHREAD_H
