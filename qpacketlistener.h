#ifndef QPACKETLISTENER_H
#define QPACKETLISTENER_H

#include <QObject>
#include "qpackettable.h"
#include "qsnifferthread.h"
#include "qpacket.h"

class QPacketListener : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString interface NOTIFY interfaceChanged    READ getInterface WRITE setInterface)
    Q_PROPERTY(QString lastError NOTIFY snifferErrorCatched READ getLastError)
    Q_PROPERTY(bool    listening NOTIFY listeningChanged    READ isListening)
public:
    explicit QPacketListener(QPacketTable* packetTable, QObject* parent = 0);
    ~QPacketListener();

protected:
    QString getInterface(void) const { return (interface_); }

    void    setInterface(QString i)
    {
      interface_ = i;
      emit interfaceChanged();
    }

    QString getLastError(void) const { return (lastError); }

    void setIsListening(bool set)
    {
      if (listening != set)
      {
        listening = set;
        emit listeningChanged();
      }
    }

    bool isListening(void) const { return (listening); }

signals:
  void interfaceChanged();
  void askedToStart();
  void askedToPause();
  void snifferErrorCatched();
  void listeningChanged();

public slots:
  void updateInterface();
  void receivedPacket();
  void catchSnifferError(QString str);
private slots:
  void pause();
  void start();

private:
  QPacketTable*  packetTable;
  QString        interface_, lastError;
  long           number;
  bool           listening;
  QSnifferThread snifferThread;
};

#endif // QPACKETLISTENER_H
