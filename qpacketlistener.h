#ifndef QPACKETLISTENER_H
#define QPACKETLISTENER_H

#include <QObject>
#include <qpackettable.h>
#include <qsnifferthread.h>
#include <qpacket.h>

class QPacketListener : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString interface NOTIFY interfaceChanged READ getInterface WRITE setInterface)
public:
    explicit QPacketListener(QPacketTable* packetTable, QObject* parent = 0);
    ~QPacketListener();

protected:
    QString getInterface(void) const { return (interface); }
    void    setInterface(QString i)
    {
      interface = i;
      emit interfaceChanged();
    }

signals:
  void interfaceChanged();
  void askedToStart();
  void askedToPause();

public slots:
  void updateInterface();
  void receivedPacket();

private slots:
  void pause();
  void start();

private:
  QPacketTable*  packetTable;
  QString        interface;
  long           number;
  QSnifferThread snifferThread;
};

#endif // QPACKETLISTENER_H