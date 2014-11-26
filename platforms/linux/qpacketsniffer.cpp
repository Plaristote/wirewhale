#include "qpacketsniffer.h"
#include <unistd.h>
#include <linux/sockios.h>
#include <linux/if_ether.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stropts.h>
#include <string.h>
#include "qpacket.h"
#include <iostream>
#include <errno.h>

QPacketSniffer::QPacketSniffer(const QString& interface_name, QObject* parent) :
    QUnixPacketSniffer(interface_name, parent),
    interface(interface_name)
{
  sock      = ::socket(PF_PACKET, SOCK_RAW, get_protocol());
  initialize_interface();
  initialize_sock_address();
  bind(sock, (struct sockaddr*)&sock_address, sizeof(sock_address));
  poll.on_event  = [this](int) { capture_packet(); };
  poll.watch(sock);
}

QPacketSniffer::~QPacketSniffer()
{
  interface.disable_promiscuous_mode(sock);
  close(sock);
}

void QPacketSniffer::initialize_interface()
{
  interface.initialize_if_index(sock);
  interface.enable_promiscuous_mode(sock);
}

void QPacketSniffer::initialize_sock_address()
{
  memset(&sock_address, 0, sizeof(sock_address));
  sock_address.sll_family   = AF_PACKET;
  sock_address.sll_ifindex  = interface.data.ifr_ifindex;
  sock_address.sll_protocol = get_protocol();
}

uint16_t QPacketSniffer::get_protocol() const
{
  return (htons(ETH_P_ALL));
}

void QPacketSniffer::run()
{
  mutex.lock();
  sniffing_thread = QThread::currentThreadId();
  must_stop       = false;
  while (!must_stop)
    poll.run();
  mutex.unlock();
}

void QPacketSniffer::wait()
{
  if (sniffing_thread != QThread::currentThreadId())
  {
    mutex.lock();
    mutex.unlock();
  }
}

void QPacketSniffer::stop()
{
  must_stop = true;
}
