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
    Q_PROPERTY(QString filter    NOTIFY filterUpdated       READ getFilter WRITE setFilter)
public:
    explicit QPacketListener(QPacketTable* packetTable, QObject* parent = 0);
    ~QPacketListener();

protected:
  QString getFilter(void) const { return (filter); }

  void setFilter(const QString name)
  {
    filter = name;
    emit filterUpdated();
  }

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
  void filterUpdated();

public slots:
  void updateInterface();
  void receivedPacket();
  void catchSnifferError(QString str);
private slots:
  void pause();
  void start();

private:
  QPacketTable*  packetTable;
  QString        interface_, lastError, filter;
  long           number;
  bool           listening;
  QSnifferThread snifferThread;
};

#endif // QPACKETLISTENER_H
