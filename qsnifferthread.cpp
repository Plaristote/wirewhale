#include "qsnifferthread.h"
#include "qpacket.h"
#include <iostream>
#include "networkinterfacelist.h"

QSnifferThread::QSnifferThread(QObject *parent) : QThread(parent)
{
  NetworkInterfaceList interface_list;

  interface_ = interface_list.first();
  sniffer    = 0;
}

void QSnifferThread::start()
{
  if (sniffer == 0)
    initializeSniffer();
  if (sniffer != 0)
  {
    mustStop = false;
    QThread::start();
  }
}

void QSnifferThread::run()
{
  startSniffing();
}

bool QSnifferThread::isSniffing()
{
  if (mutex.tryLock())
  {
    mutex.unlock();
    return (false);
  }
  return (true);
}

void QSnifferThread::changeInterface(QString interface)
{
  this->interface_ = interface;
  if (isSniffing())
  {
    stopSniffing();
    mutex.lock();
    deleteSniffer();
    start();
    mutex.unlock();
  }
}

QPacketSniffer::PacketFilter filterPacketType(QPacketSniffer::Packet::EtherType ether_type)
{
  return [ether_type](QPacketSniffer::Packet packet) -> bool {
    return packet.get_ether_type() != ether_type;
  };
}

void QSnifferThread::initializeSniffer()
{
  try
  {
    number  = 0;
    sniffer = new QPacketSniffer(interface_, this);
    //sniffer->addFilter(filterPacketType(QPacketSniffer::Packet::ARP));
    connect(sniffer, SIGNAL(packetsReceived()), this, SIGNAL(packetReceived()));
  }
  catch (const std::runtime_error& e)
  {
    emit snifferFailedToStart(QString(e.what()));
  }
}

void QSnifferThread::startSniffing()
{
  mutex.lock();
  sniffer->run();
  mutex.unlock();
}

void QSnifferThread::stopSniffing()
{
  sniffer->stop();
}

void QSnifferThread::deleteSniffer()
{
  if (sniffer != 0)
  {
    sniffer->stop();
    sniffer->wait();
    delete sniffer;
    sniffer = 0;
  }
}

QVector<QPacket> QSnifferThread::receivedPackets()
{
  QVector<QPacket> pending_packets;

  return (sniffer->pullPendingPackets(pending_packets));
}
