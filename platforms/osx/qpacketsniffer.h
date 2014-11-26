#ifndef  OSX_QPACKET_SNIFFER_H
# define OSX_QPACKET_SNIFFER_H

# include "qabstractpacketsniffer.h"
# include <sys/select.h>
# include <sys/socket.h>
# include <net/if.h>
# include <net/ndrv.h>
# include <functional>
# include <QMutex>

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
      void       reset_fds();

      QList<int>     read_set_fds;
      fd_set         read_set;
      int            max_fd;
      struct timeval timeout;
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
    QPacketSniffer(const QString& interface_name, QObject* parent);
    ~QPacketSniffer();

    void run();
    void wait();
    void stop();
private:
    void    initialize_sock_address();
    void    initialize_protocol();
    void    capture_packet();

    int                  sock;
    struct sockaddr_ndrv sock_address;
    Poll                 poll;
    bool                 must_stop;
    QMutex               mutex;
    Qt::HANDLE           sniffing_thread;
};

#endif
