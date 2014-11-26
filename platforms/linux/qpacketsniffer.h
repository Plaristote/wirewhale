#ifndef  LINUX_QPACKET_SNIFFER_H
# define LINUX_QPACKET_SNIFFER_H

# include "qabstractpacketsniffer.h"
# include <sys/socket.h>
# include <functional>

# include <linux/if.h>
# include <linux/if_packet.h>
# include <sys/epoll.h>

# include <QMutex>
# include <QThread>

class QPacketSniffer : public QAbstractPacketSniffer
{
    Q_OBJECT

    class Poll
    {
    public:
      Poll();
      ~Poll();

      void watch(int fd);
      void run();

      std::function<void (int fd)> on_event;

    private:
      int                 efd;
      unsigned int        max_events;
      struct epoll_event* events;
    };

    struct Interface
    {
      Interface(const QString& name);
      ~Interface();

      void initialize_if_index(int fd);
      void enable_promiscuous_mode(int fd);
      void disable_promiscuous_mode(int fd);

      struct ifreq data;
    private:
      void raise(const QString& message);
    };

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

public:
    QPacketSniffer(const QString& interface_name, QObject* parent = 0);
    ~QPacketSniffer();

    void run();
    void stop();
    void wait();

private:
    uint16_t get_protocol() const;
    void     initialize_interface();
    void     initialize_sock_address();
    void     initialize_poll();
    void     capture_packet();

    int                sock;
    Interface          interface;
    struct sockaddr_ll sock_address;
    Poll               poll;
    bool               must_stop;
    QMutex             mutex;
    Qt::HANDLE         sniffing_thread;
};

#endif
