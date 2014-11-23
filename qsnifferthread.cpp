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
  try
  {
    number  = 0;
    sniffer = new Sniffer(interface.toStdString());
    sniffer->set_timeout(5000);
  }
  catch (const std::runtime_error& e)
  {
    emit snifferFailedToStart(QString(e.what()));
  }
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
  if (sniffer != 0)
  {
    sniffer->stop_sniff();
    delete sniffer;
    sniffer = 0;
  }
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
    case Tins::PDU::LOOPBACK:
      return "LOOPBACK";
    case Tins::PDU::PPPOE:
        return "PPPOE";
    case Tins::PDU::SNAP:
        return "SNAP";
    case Tins::PDU::IP:
        return "IP";
    case Tins::PDU::LLC:
        return "LLC";
    case Tins::PDU::BOOTP:
        return "BOOTP";
    case Tins::PDU::EAPOL:
        return "EAPOL";
    case Tins::PDU::RC4EAPOL:
        return "RC4EAPOL";
    case Tins::PDU::RSNEAPOL:
        return "RSNEAPOL";
    case Tins::PDU::DNS:
        return "DNS";
    case Tins::PDU::SLL:
        return "SLL";
    case Tins::PDU::PPI:
        return "PPI";
  }
  return (QString::number(code));
}

bool QSnifferThread::snifferCallback(Tins::PDU& pdu)
{
  const IP&  ip  = pdu.rfind_pdu<IP>();
  QPacket    packet;

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
