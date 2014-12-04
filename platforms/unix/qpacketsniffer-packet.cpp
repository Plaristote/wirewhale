#include "qpacketsniffer.h"
#include <unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <net/ethernet.h>
#include "endianness.h"

#if defined(__linux__)
# include <linux/tcp.h>
# include <linux/udp.h>
# include <linux/icmp.h>
# include <netinet/ether.h>
#elif defined(__APPLE__)
# include <net/if_arp.h>
#endif

#if !defined(__linux__)
struct iphdr {
#if USE_LITTLE_ENDIAN
        uint8_t ihl:4,
                version:4;
#else
        uint8_t version:4,
                ihl:4;
#endif
        uint8_t   tos;
        uint16_t  tot_len;
        uint16_t  id;
        uint16_t  frag_off;
        uint8_t   ttl;
        uint8_t   protocol;
        uint8_t   check;
        uint32_t  saddr;
        uint32_t  daddr;
        /*The options start here. */
};

struct tcphdr {
        uint16_t   source;
        uint16_t   dest;
        uint32_t   seq;
        uint32_t   ack_seq;
#if USE_LITTLE_ENDIAN
        uint16_t   res1:4,
                doff:4,
                fin:1,
                syn:1,
                rst:1,
                psh:1,
                ack:1,
                urg:1,
                ece:1,
                cwr:1;
#else
        uint16_t doff:4,
                res1:4,
                cwr:1,
                ece:1,
                urg:1,
                ack:1,
                psh:1,
                rst:1,
                syn:1,
                fin:1;
#endif
        uint16_t   window;
        uint16_t   check;
        uint16_t   urg_ptr;
};

struct udphdr {
    u_short	sport;	/* source port */
    u_short	dport;	/* destination port */
    u_short	len;	/* udp length */
    u_short	sum;	/* udp checksum */
};
#endif

QPacketSniffer::Packet::Packet()
{
  memset(buffer, 0, sizeof(buffer));
  eth    = (struct ether_header*)buffer;
  ip     = (struct iphdr*)(buffer + packet_offset_ip_header());
  arp    = (struct arphdr*)(ip);
  length = MAX_PACKET_LENGTH;
}

bool QPacketSniffer::Packet::has_supported_type() const
{
  switch (get_ether_type())
  {
    case IPv4:
    case IPv6:
    case ARP:
      return true;
  default:
      break ;
  }
  return false;
}

QByteArray arpOperationCodeToString(short opCode)
{
  switch (opCode)
  {
  case 1:
    return "REQUEST";
  case 2:
    return "REPLY";
  case 3:
    return "request Reverse";
  case 4:
    return "reply Reverse";
  case 5:
    return "DRARP-Request";
  case 6:
    return "DRARP-Reply";
  case 7:
    return "DRARP-Error";
  case 8:
    return "InARP-Request";
  case 9:
    return "InARP-Reply";
  case 10:
    return "APP-NAK";
  }
  return QByteArray(QString("unknown operation code '" + QString::number(opCode) + '\'').toLatin1());
}

QByteArray QPacketSniffer::Packet::data() const
{
  QByteArray str;

  str += QByteArray("# Mac Addresses\n");
  str += QByteArray("Sender: ") + get_source_mac() + '\n';
  str += QByteArray("Dest:   ") + get_destination_mac() + "\n\n";
  if (has_ip_type())
  {
    if (ip->protocol == IPPROTO_TCP)
    {
      struct tcphdr* tcp      = (struct tcphdr*)(buffer + packet_offset_xcp_header());
      const char*    payload  = (const char*)((unsigned char*)(tcp) + (tcp->doff * 4));
      size_t         max_size = (buffer + ip->tot_len - payload) / sizeof(char);

      str += QByteArray(payload, max_size);
    }
    else if (ip->protocol == IPPROTO_UDP)
    {
      struct udphdr* udp      = (struct udphdr*)(buffer + packet_offset_xcp_header());
      const char*    payload  = (const char*)((unsigned char*)(udp) + sizeof(struct udphdr));
      size_t         max_size = udp->len;

      str += QByteArray(payload, max_size);
    }
    else if (ip->protocol == IPPROTO_ICMP)
    {
    }
  }
  else if (get_ether_type() == ARP)
  {
    QString body;

    body += "Format Hardware Address: " + QString::number(arp->ar_hrd) + '\n';
    body += "Format Protocol Address: " + QString::number(arp->ar_pro) + '\n';
    body += "Length Hardware Address: " + QString::number(arp->ar_hln) + '\n';
    body += "Length Protocol Address: " + QString::number(arp->ar_pln) + '\n';
    body += "Operation Code: " + arpOperationCodeToString(arp->ar_op);
    str +=  body.toLatin1();
  }
  return str;
}


bool QPacketSniffer::Packet::has_ip_type() const
{
  switch (get_ether_type())
  {
  case IPv4:
  case IPv6:
    return true;
  default:
    break ;
  }
  return false;
}

QPacketSniffer::Packet::EtherType QPacketSniffer::Packet::get_ether_type(void) const
{
  return (EtherType)(ntohs(eth->ether_type));
}

QString QPacketSniffer::Packet::get_source_mac(void) const
{
  return ether_ntoa((struct ether_addr*)(eth->ether_shost));
}

QString QPacketSniffer::Packet::get_destination_mac(void) const
{
  return ether_ntoa((struct ether_addr*)(eth->ether_dhost));
}

QString QPacketSniffer::Packet::get_source_ip(void) const
{
  if (has_ip_type())
    return IpPacket(this).get_source_ip();
  else if (get_ether_type() == ARP)
    return ArpPacket(this).get_source_ip();
  return "";
}

QByteArray QPacketSniffer::Packet::ArpPacket::get_source_ip() const
{
  const unsigned char* ar_sip = sender_ip_address_ptr();

  return get_ip_at_address(ar_sip, arp->ar_pln);
}

QByteArray QPacketSniffer::Packet::ArpPacket::get_destination_ip() const
{
  const unsigned char* ar_dip = target_ip_address_ptr();

  return get_ip_at_address(ar_dip, arp->ar_pln);
}

const unsigned char* QPacketSniffer::Packet::ArpPacket::sender_hardware_address_ptr() const
{
  return (const unsigned char*)(arp) + sizeof(arphdr);
}

const unsigned char* QPacketSniffer::Packet::ArpPacket::sender_ip_address_ptr() const
{
  return sender_hardware_address_ptr() + (sizeof(unsigned char) * arp->ar_hln);
}

const unsigned char* QPacketSniffer::Packet::ArpPacket::target_hardware_address_ptr() const
{
  return sender_ip_address_ptr() + (sizeof(unsigned char) * arp->ar_pln);
}

const unsigned char* QPacketSniffer::Packet::ArpPacket::target_ip_address_ptr() const
{
  return target_hardware_address_ptr() + (sizeof(unsigned char) * arp->ar_hln);
}

QByteArray QPacketSniffer::Packet::ArpPacket::get_ip_at_address(const unsigned char* ptr, unsigned char len) const
{
  QByteArray ip_string;

  for (unsigned char i = 0 ; i < len ; ++i)
  {
    if (i > 0)
      ip_string += '.';
    ip_string += QByteArray::number(ptr[i]);
  }
  return ip_string;
}

QPacketSniffer::Packet::ArpPacket::ArpPacket(const Packet* packet) : packet(*packet)
{
  arp = (struct arphdr*)(packet->buffer + Packet::packet_offset_ip_header());
}

QPacketSniffer::Packet::IpPacket::IpPacket(const Packet* packet) : packet(*packet)
{
  ip = (struct iphdr*)(packet->buffer + Packet::packet_offset_ip_header());
}

QByteArray QPacketSniffer::Packet::IpPacket::get_source_ip() const
{
  char ip_string[16];

  inet_ntop(AF_INET, &ip->saddr, ip_string, 16);
  return ip_string;
}

QByteArray QPacketSniffer::Packet::IpPacket::get_destination_ip() const
{
  char ip_string[16];

  inet_ntop(AF_INET, &ip->daddr, ip_string, 16);
  return ip_string;
}


QString QPacketSniffer::Packet::get_destination_ip(void) const
{
  if (has_ip_type())
    return Packet::IpPacket(this).get_destination_ip();
  else if (get_ether_type() == ARP)
    return Packet::ArpPacket(this).get_destination_ip();
  return "";
}

QString QPacketSniffer::Packet::get_protocol(void) const
{
  if (has_ip_type())
  {
    switch (ip->protocol)
    {
    case IPPROTO_TCP:
      return "tcp";
    case IPPROTO_UDP:
      return "udp";
    case IPPROTO_ICMP:
      return "icmp";
    case IPPROTO_ICMPV6:
        return "icmpv6";
    case IPPROTO_IGMP:
      return "igmp";
    case IPPROTO_RSVP:
      return "rsvp";
    case IPPROTO_SCTP:
      return "sctp";
#ifdef IPPROTO_DCCP
    case IPPROTO_DCCP:
      return "dccp";
#endif
    case IPPROTO_EGP:
        return "egp";
    case IPPROTO_ESP:
      return "esp";
    case IPPROTO_AH:
      return "ah";
#ifdef IPPROTO_COMP
    case IPPROTO_COMP:
      return "comp";
#endif
#ifdef IPPROTO_BEETPH
    case IPPROTO_BEETPH:
      return "beethph";
#endif
    case IPPROTO_DSTOPTS:
      return "dstopts";
    case IPPROTO_FRAGMENT:
      return "fragment";
    case IPPROTO_GRE:
      return "gre";
    case IPPROTO_ENCAP:
      return "encap";
    case IPPROTO_IDP:
      return "idp";
    case IPPROTO_IP:
      return "ip";
    case IPPROTO_IPV6:
      return "ipv6";
    case IPPROTO_IPIP:
      return "ipip";
    case IPPROTO_TP:
      return "tp";
    case IPPROTO_RAW:
      return "raw";
    case 112:
      return "vrrp";
    case IPPROTO_PIM:
      return "pim";
    default:
      break ;
    }
    return QString::number(ip->protocol);
  }
  else if (get_ether_type() == ARP)
    return "arp";
  return "";
}

size_t QPacketSniffer::Packet::packet_offset_ip_header()
{
  return sizeof(ether_header);
}

size_t QPacketSniffer::Packet::packet_offset_xcp_header()
{
  return packet_offset_ip_header() + sizeof(struct iphdr);
}
