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
#include<sys/types.h>

QPacketSniffer::QPacketSniffer(const QString& interface_name, QObject* parent) : QAbstractPacketSniffer(interface_name, parent)
{
  struct ifreq       interface;
  //struct sockaddr_ll sock_address;

  sock == ::socket(PF_NDRV, SOCK_RAW, 0);
 // bind(sock, (struct sockaddr*)&sock_address, sizeof(sock_address));
}

QPacketSniffer::~QPacketSniffer()
{
  close(sock);
}

void QPacketSniffer::run()
{
}
