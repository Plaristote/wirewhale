#ifndef QABSTRACTPACKETSNIFFER_H
#define QABSTRACTPACKETSNIFFER_H

#include <QObject>
#include <QMutex>
#include <QVector>
#include <QList>
#include "qpacket.h"
#include <functional>

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
};

#endif // QABSTRACTPACKETSNIFFER_H
