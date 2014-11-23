#include "qpacketlistener.h"
#include <tins/tins.h>
#include <ctime>
#include <iostream>
using namespace Tins;

QPacketListener::QPacketListener(QPacketTable* packetTable, QObject* parent) :
    QObject(parent), packetTable(packetTable)
{
  connect(this, SIGNAL(interfaceChanged()), this, SLOT(updateInterface()));
  connect(&snifferThread, SIGNAL(packetReceived()), this, SLOT(receivedPacket()), Qt::QueuedConnection);
  //snifferThread.start();
}

QPacketListener::~QPacketListener()
{
}

void QPacketListener::updateInterface()
{
  QString interface = property("interface").toString();

  snifferThread.changeInterface(interface);
}

void QPacketListener::receivedPacket()
{
  QVector<QPacket> packets = snifferThread.receivedPackets();

  if (packets.count() > 0)
    packetTable->addPackets(packets);
}
