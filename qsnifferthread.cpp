#include "qsnifferthread.h"
#include <qpacket.h>
#include <iostream>

using namespace Tins;

QSnifferThread* QSnifferThread::self = 0;

QSnifferThread::QSnifferThread(QObject *parent) :
    QThread(parent)
{
  self      = this;
  mustStop  = false;
  interface = "enp0s25";
  initializeSniffer();
}

void QSnifferThread::run()
{
  startSniffing();
}

void QSnifferThread::changeInterface(QString interface)
{
  this->interface = interface;
  mustStop        =  true;
  mutex.lock();
  sniffer->stop_sniff();
  delete sniffer;
  initializeSniffer();
  mustStop        = false;
  mutex.unlock();
  start();
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
  sniffer->sniff_loop(QSnifferThread::snifferCallback);
  mutex.unlock();
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
  return ("?");
}

bool QSnifferThread::snifferCallback(Tins::PDU& pdu)
{
  const IP&  ip  = pdu.rfind_pdu<IP>();
  const TCP& tcp = pdu.rfind_pdu<TCP>();
  QPacket    packet;

  packet.number      = ++(self->number);
  packet.time        = time(0);
  packet.source      = QString::fromStdString(ip.src_addr().to_string());
  packet.destination = QString::fromStdString(ip.dst_addr().to_string());
  packet.protocol    = protocol2String(ip.protocol());
  packet.length      = ip.size();
  self->list_mutex.lock();
  self->received_packets << packet;
  self->list_mutex.unlock();
  emit self->packetReceived();
  return (!self->mustStop);
}
