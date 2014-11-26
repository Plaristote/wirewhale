#ifndef QUNIXPACKETSNIFFER_H
# define QUNIXPACKETSNIFFER_H

# include "qabstractpacketsniffer.h"

class QUnixPacketSniffer : public QAbstractPacketSniffer
{
    Q_OBJECT
protected:
    struct Packet
    {
      Packet();

      enum EtherType
      {
        ETHER_TYPE_UNKNOWN = 0,
        IPv4 = 0x0800,
        IPv6 = 0x86DD,
        ARP  = 0x0806
      };

      bool has_supported_type()        const;
      bool has_ip_type()               const;
      EtherType get_ether_type(void)   const;
      QString get_source_ip(void)      const;
      QString get_destination_ip(void) const;
      QString get_protocol(void)       const;

      static size_t packet_offset_ip_header();
      static size_t packet_offset_xcp_header();

      char                 buffer[65535];
      struct ether_header* eth;
      struct iphdr*        ip;
    };

    explicit QUnixPacketSniffer(const QString& interface_name, QObject *parent = 0);

    void capture_packet();

    int sock;
};

#endif // QUNIXPACKETSNIFFER_H
