#include "qpacketlistener.h"
#include <ctime>
#include <iostream>

QPacketListener::QPacketListener(QPacketTable* packetTable, QObject* parent) :
    QObject(parent), packetTable(packetTable)
{
  connect(this, SIGNAL(askedToStart()), this, SLOT(start()));
  connect(this, SIGNAL(askedToPause()), &snifferThread, SLOT(stopSniffing()));
  connect(this, SIGNAL(interfaceChanged()), this, SLOT(updateInterface()));
  connect(&snifferThread, SIGNAL(snifferFailedToStart(QString)), this, SLOT(catchSnifferError(QString)), Qt::QueuedConnection);
  connect(&snifferThread, SIGNAL(packetReceived()),              this, SLOT(receivedPacket()),           Qt::QueuedConnection);
  connect(&snifferThread, SIGNAL(finished()),                    this, SLOT(pause()),                    Qt::QueuedConnection);
}

QPacketListener::~QPacketListener()
{
}

void QPacketListener::updateInterface()
{
  QString interface = property("interface").toString();

  std::cout << "Updated interface to " << interface.toStdString() << std::endl;
  snifferThread.changeInterface(interface);
}

void QPacketListener::receivedPacket()
{
  QVector<QPacket> packets = snifferThread.receivedPackets();

  if (packets.count() > 0)
    packetTable->addPackets(packets);
}

void QPacketListener::start()
{
  packetTable->clear();
  updateInterface();
  if (!snifferThread.isSniffing())
    snifferThread.start();
  setIsListening(true);
}

void QPacketListener::pause()
{
  setIsListening(false);
}

void QPacketListener::catchSnifferError(QString str)
{
  lastError = str;
  emit snifferErrorCatched();
  setIsListening(false);
}
