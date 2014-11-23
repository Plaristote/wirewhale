#ifndef QPACKETLISTENER_H
#define QPACKETLISTENER_H

#include <QObject>
#include <qpackettable.h>
#include <qsnifferthread.h>
#include <qpacket.h>

class QPacketListener : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString interface NOTIFY interfaceChanged    READ getInterface WRITE setInterface)
    Q_PROPERTY(QString lastError NOTIFY snifferErrorCatched READ getLastError)
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

    QString getLastError(void) const { return (lastError); }

signals:
  void interfaceChanged();
  void askedToStart();
  void askedToPause();
  void snifferErrorCatched();

public slots:
  void updateInterface();
  void receivedPacket();
  void catchSnifferError(QString str);
private slots:
  void pause();
  void start();

private:
  QPacketTable*  packetTable;
  QString        interface, lastError;
  long           number;
  QSnifferThread snifferThread;
};

#endif // QPACKETLISTENER_H
