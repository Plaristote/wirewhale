#include "qsnifferthread.h"
#include <qpacket.h>
#include <iostream>

using namespace Tins;

QSnifferThread::QSnifferThread(QObject *parent) :
    QThread(parent)
{
  interface = "enp0s25";
  sniffer   = 0;
}

void QSnifferThread::start()
{
  if (sniffer == 0)
    initializeSniffer();
  mustStop = false;
  QThread::start();
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
  this->interface = interface;
  if (isSniffing())
  {
    stopSniffing();
    mutex.lock();
    deleteSniffer();
    start();
    mutex.unlock();
  }
}

void QSnifferThread::initializeSniffer()
{
  number  = 0;
  sniffer = new Sniffer(interface.toStdString());
  sniffer->set_timeout(5000);
}

void QSnifferThread::startSniffing()
{
  mutex.lock();
  sniffer->sniff_loop([this](Tins::PDU& pdu) -> bool { return snifferCallback(pdu); });
  mutex.unlock();
}

void QSnifferThread::stopSniffing()
{
  mustStop = true;
}

void QSnifferThread::deleteSniffer()
{
  sniffer->stop_sniff();
  delete sniffer;
  sniffer = 0;
}

QVector<QPacket> QSnifferThread::receivedPackets()
{
  QVector<QPacket> copy;

  list_mutex.lock();
  copy = received_packets.toVector();
  received_packets.clear();
  list_mutex.unlock();
  return (copy);
}

QString protocol2String(u_int8_t code)
{
  switch (code)
  {
    case Tins::PDU::TCP:
      return "TCP";
    case Tins::PDU::UDP:
      return "UDP";
    case Tins::PDU::STP:
      return "STP";
    case Tins::PDU::DHCP:
      return "DHCP";
    case Tins::PDU::DHCPv6:
      return "DHCPv6";
    case Tins::PDU::ARP:
      return "ARP";
    case Tins::PDU::ICMP:
      return "ICMP";
    case Tins::PDU::ICMPv6:
      return "ICMPv6";
  }
  return (QString::number(code));
}

bool QSnifferThread::snifferCallback(Tins::PDU& pdu)
{
  const IP&  ip  = pdu.rfind_pdu<IP>();
  QPacket    packet;

  sniffer->set_filter("");
  packet.number      = ++(number);
  packet.time        = time(0);
  packet.source      = QString::fromStdString(ip.src_addr().to_string());
  packet.destination = QString::fromStdString(ip.dst_addr().to_string());
  packet.protocol    = protocol2String(ip.protocol());
  packet.length      = ip.size();
  list_mutex.lock();
  received_packets << packet;
  list_mutex.unlock();
  emit packetReceived();
  return (!mustStop);
}
