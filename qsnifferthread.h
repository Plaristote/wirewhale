#ifndef QSNIFFERTHREAD_H
#define QSNIFFERTHREAD_H

#include <QThread>
#include <QMutex>
#include <tins/tins.h>
#include <qpacket.h>
#include <QList>
#include <QVector>

class QSnifferThread : public QThread
{
    Q_OBJECT
public:
    explicit QSnifferThread(QObject *parent = 0);

    QVector<QPacket> receivedPackets();

signals:
  void packetReceived();

public slots:
  void startSniffing();
  void changeInterface(QString interface);

private:
  void            run();
  void            initializeSniffer();
  static bool     snifferCallback(Tins::PDU&);

  QString                interface;
  QMutex                 mutex;
  bool                   mustStop;
  Tins::Sniffer*         sniffer;
  long                   number;
  static QSnifferThread* self;

  QMutex         list_mutex;
  QList<QPacket> received_packets;
};

#endif // QSNIFFERTHREAD_H
