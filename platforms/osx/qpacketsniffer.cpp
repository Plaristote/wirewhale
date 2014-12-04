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
}

QPacketSniffer::~QPacketSniffer()
{
  std::cout << "closing bpf device" << std::endl;
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
  std::cout << "socket: " << sock << std::endl;
}

void QPacketSniffer::initialize_interface()
{
  memset(&interface, 0, sizeof(interface));
  strncpy(interface.ifr_name, interface_name.toUtf8().data(), interface_name.length());
  if (ioctl(sock, BIOCSETIF, &interface) > 0)
    throw std::runtime_error("cannot initialize interface `" + interface_name.toUtf8() + "`: " + QByteArray(strerror(errno)));
}

void QPacketSniffer::initialize_bpf()
{
  bool success = true;

  bpf_buffer_length = 1;
  success = ioctl(sock, BIOCIMMEDIATE, &bpf_buffer_length) >= 0;
  if (success == false)
    throw std::runtime_error(QByteArray("cannot set BIOCIMMEDIATE on BPF device: ") + QByteArray(strerror(errno)));
  success = ioctl(sock, BIOCGBLEN,     &bpf_buffer_length) >= 0;
  if (success == false)
    throw std::runtime_error(QByteArray("cannot set BIOCGBLEN on BPF device: ") + QByteArray(strerror(errno)));
  std::cout << "bpf buffer length is " << bpf_buffer_length << std::endl;
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
        ptr += BPF_WORDALIGN(bpf_packet->bh_hdrlen + bpf_packet->bh_caplen);
      }
    }
    else
    {
      std::cout << "Cannot read on BPF device: " << QByteArray(strerror(errno)).constData() << std::endl;
      must_stop = true;
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
