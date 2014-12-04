#ifndef QABSTRACTPACKETSNIFFER_H
#define QABSTRACTPACKETSNIFFER_H

#include <QObject>
#include <QMutex>
#include <QVector>
#include <QList>
#include "qpacket.h"
#include <functional>

#define MAX_PACKET_LENGTH 65535

class QAbstractPacketSniffer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString interface_name READ getInterfaceName)
public:
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

      QByteArray data() const;

      bool has_supported_type()         const;
      bool has_ip_type()                const;
      EtherType get_ether_type(void)    const;
      QString get_source_mac(void)      const;
      QString get_destination_mac(void) const;
      QString get_source_ip(void)       const;
      QString get_destination_ip(void)  const;
      QString get_protocol(void)        const;

      static size_t packet_offset_ip_header();
      static size_t packet_offset_xcp_header();

      char                 buffer[MAX_PACKET_LENGTH];
      struct ether_header* eth;
      unsigned short       length;

      struct IpPacket {
        IpPacket(const Packet*);

        QByteArray get_source_ip()      const;
        QByteArray get_destination_ip() const;

        const Packet& packet;
        struct iphdr* ip;
      };

      struct ArpPacket {
        ArpPacket(const Packet*);

        QByteArray get_source_ip() const;
        QByteArray get_destination_ip() const;
        QByteArray get_ip_at_address(const unsigned char* ptr, unsigned char length) const;
        const unsigned char* sender_hardware_address_ptr()  const;
        const unsigned char* sender_ip_address_ptr()       const;
        const unsigned char* target_hardware_address_ptr() const;
        const unsigned char* target_ip_address_ptr()       const;

        const Packet& packet;
        struct arphdr* arp;
      };

      struct iphdr*        ip;
      struct arphdr*       arp;
    };

    typedef std::function<bool (const Packet&)> PacketFilter;
    explicit QAbstractPacketSniffer(const QString& interface_name, QObject *parent = 0);

    const QString& getInterfaceName(void) { return (interface_name); }

    virtual void run(void)  = 0;
    virtual void wait(void) = 0;
    virtual void stop(void) = 0;

    QVector<QPacket>    pullPendingPackets(QVector<QPacket> list);

    void clearFilters()                 { filters.clear();           }
    void addFilter(PacketFilter filter) { filters.push_back(filter); }
signals:
    void packetsReceived();

public slots:

protected:
    bool                runFilters(const Packet&) const;

    QString             interface_name;
    QList<QPacket>      pending_packets;
    QList<PacketFilter> filters;
    QMutex              pending_packets_mutex;
    short               max_captured_packets;
    unsigned int        captured_packets;
};

#endif // QABSTRACTPACKETSNIFFER_H
