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

// http://www.lists.apple.com/archives/darwin-dev/2011/Jan/msg00049.html

using namespace std;

QPacketSniffer::QPacketSniffer(const QString& interface_name, QObject* parent) : QAbstractPacketSniffer(interface_name, parent)
{
  struct sockaddr_ndrv sock_address;

  sock = ::socket(PF_NDRV, SOCK_RAW, 0);

  initialize_sock_address();
  bind(sock, (struct sockaddr*)&sock_address, sizeof(sock_address));
}

QPacketSniffer::~QPacketSniffer()
{
  close(sock);
}

void QPacketSniffer::initialize_sock_address()
{
  int name_length = interface_name.length();

  strncpy((char*)sock_address.snd_name, interface_name.toStdString().c_str(), name_length > IFNAMSIZ ? IFNAMSIZ : name_length);
  std::cout << "initialize sock address " << sock_address.snd_name << std::endl;
  sock_address.snd_len    = sizeof(sock_address);
  sock_address.snd_family = AF_NDRV;
}

void QPacketSniffer::run()
{
}

void QPacketSniffer::wait()
{
}
