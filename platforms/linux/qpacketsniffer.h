#ifndef  LINUX_QPACKET_SNIFFER_H
# define LINUX_QPACKET_SNIFFER_H

# include "qabstractpacketsniffer.h"
# include <sys/socket.h>
# include <functional>

# include <linux/if.h>
# include <linux/if_packet.h>
# include <sys/epoll.h>

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

public:
    QPacketSniffer(const QString& interface_name, QObject* parent = 0);
    ~QPacketSniffer();

    void run();
    void stop();

private:
    uint16_t get_protocol() const;
    void     initialize_interface();
    void     initialize_sock_address();
    void     initialize_poll();
    void     capture_packet();

    static size_t packet_offset_ip_header();
    static size_t packet_offset_xcp_header();

    QString            interface_name;
    int                sock;
    struct ifreq       interface;
    struct sockaddr_ll sock_address;
    Poll               poll;
    bool               must_stop;
};

#endif
