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
    bool             isSniffing();

signals:
  void packetReceived();

public slots:
  void start();
  void startSniffing();
  void stopSniffing();
  void changeInterface(QString interface);

private:
  void           run();
  void           initializeSniffer();
  void           deleteSniffer();
  bool           snifferCallback(Tins::PDU&);

  QString        interface;
  QMutex         mutex;
  bool           mustStop;
  Tins::Sniffer* sniffer;
  long           number;

  QMutex         list_mutex;
  QList<QPacket> received_packets;
};

#endif // QSNIFFERTHREAD_H
