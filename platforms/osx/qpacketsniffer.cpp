#include "platforms/osx/qpacketsniffer.h"
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h> //For standard things
#include <stdlib.h>    //malloc
#include <string.h>    //strlen
#include <unistd.h>

#include <net/if.h>
#include <netinet/ip_icmp.h>   //Provides declarations for icmp header
#include <netinet/udp.h>   //Provides declarations for udp header
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <netinet/if_ether.h>  //For ETH_P_ALL
#include <net/ethernet.h>  //For ether_header
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <iostream>

#include <net/bpf.h>
#include <fcntl.h>

#include <QThread>

// http://bastian.rieck.ru/howtos/bpf/

using namespace std;

QPacketSniffer::QPacketSniffer(const QString& interface_name, QObject* parent) : QUnixPacketSniffer(interface_name, parent)
{
  open_bpf();
  initialize_interface();
  initialize_bpf();
  poll.on_event = [this](int) { capture_packet(); };
}

QPacketSniffer::~QPacketSniffer()
{
  close(sock);
}

void QPacketSniffer::open_bpf()
{
  for (short i = 0 ; i < 99 ; ++i)
  {
    QByteArray device_path = "/dev/bpf" + QByteArray::number(i);

    if ((sock = open(device_path.constData(), O_RDWR)) != -1)
      break ;
  }
  if (sock == -1)
    throw std::runtime_error("cannot open any bpf device");
}

void QPacketSniffer::initialize_interface()
{
  memset(&interface, 0, sizeof(interface));
  strncpy(interface.ifr_name, interface_name.toUtf8().data(), interface_name.length());
}

void QPacketSniffer::initialize_bpf()
{
  bool success = true;

  success = success && ioctl(sock, BIOCIMMEDIATE, &bpf_buffer_length) == -1;
  success = success && ioctl(sock, BIOCGBLEN,     &bpf_buffer_length) == -1;
  if (success == false)
    throw std::runtime_error(strerror(errno));
}

void QPacketSniffer::run()
{
  mutex.lock();
  sniffing_thread = QThread::currentThreadId();
  must_stop = false;
  bpf();
  mutex.unlock();
}

void QPacketSniffer::bpf()
{
  int read_bytes = 0;
  struct bpf_hdr* buffer = new bpf_hdr[bpf_buffer_length];

  while (!must_stop)
  {
    memset(buffer, 0, bpf_buffer_length);
    if ((read_bytes = read(sock, buffer, bpf_buffer_length)) > 0)
    {
      char* ptr = reinterpret_cast<char*>(buffer);
      while (ptr < (reinterpret_cast<char*>(buffer) + read_bytes))
      {
        Packet          packet;
        struct bpf_hdr* bpf_packet = reinterpret_cast<struct bpf_hdr*>(ptr);

        strncpy(packet.buffer, (const char*)bpf_packet + bpf_packet->bh_hdrlen, packet.length);
        append_packet(packet);
      }
    }
  }
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
