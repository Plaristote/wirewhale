#include "qpacketsniffer.h"
#include <unistd.h>
#include <linux/sockios.h>
#include <linux/if_ether.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stropts.h>
#include <string.h>
#include <iostream>
#include <errno.h>

QPacketSniffer::Packet::Packet()
{
  memset(buffer, 0, sizeof(buffer));
  eth = (struct ether_header*)buffer;
  ip  = (struct iphdr*)(buffer + packet_offset_ip_header());
  //struct tcphdr*       tcp = (struct tcphdr*)(packet + packet_offset_xcp_header());
}

bool QPacketSniffer::Packet::has_supported_type() const
{
  switch (get_ether_type())
  {
    case IPv4:
    case IPv6:
    case ARP:
      return true;
  }
  return false;
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

QString QPacketSniffer::Packet::get_source_ip(void) const
{
  if (has_ip_type())
  {
    char ip_string[16];

    inet_ntop(AF_INET, &ip->saddr, ip_string, 16);
    return ip_string;
  }
  return "";
}

QString QPacketSniffer::Packet::get_destination_ip(void) const
{
  if (has_ip_type())
  {
      char ip_string[16];

    inet_ntop(AF_INET, &ip->daddr, ip_string, 16);
    return ip_string;
  }
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
    case IPPROTO_DCCP:
      return "dccp";
    case IPPROTO_EGP:
        return "egp";
    case IPPROTO_ESP:
      return "esp";
    case IPPROTO_AH:
      return "ah";
    case IPPROTO_COMP:
      return "comp";
    case IPPROTO_BEETPH:
      return "beethph";
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
