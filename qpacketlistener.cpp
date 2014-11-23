#include "qpacketlistener.h"
#include <tins/tins.h>
#include <ctime>
#include <iostream>
using namespace Tins;

QPacketListener::QPacketListener(QPacketTable* packetTable, QObject* parent) :
    QObject(parent), packetTable(packetTable)
{
  connect(this, SIGNAL(askedToStart()), this, SLOT(start()));
  connect(this, SIGNAL(askedToPause()), &snifferThread, SLOT(stopSniffing()));
  connect(this, SIGNAL(interfaceChanged()), this, SLOT(updateInterface()));
  connect(&snifferThread, SIGNAL(packetReceived()), this, SLOT(receivedPacket()), Qt::QueuedConnection);
}

QPacketListener::~QPacketListener()
{
}

void QPacketListener::updateInterface()
{
  std::cout << "Updated interface !" << std::endl;
  QString interface = property("interface").toString();

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
  updateInterface();
  if (!snifferThread.isSniffing())
    snifferThread.start();
}

void QPacketListener::pause()
{
}
