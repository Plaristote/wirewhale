#ifndef  OSX_QPACKET_SNIFFER_H
# define OSX_QPACKET_SNIFFER_H

# include "qabstractpacketsniffer.h"
# include <sys/select.h>
# include <sys/socket.h>
# include <net/if.h>
# include <net/ndrv.h>
# include <functional>

class QPacketSniffer : public QAbstractPacketSniffer
{
    Q_OBJECT

    class Poll
    {
      Poll();
      ~Poll();

      void watch(int fd);
      void run();

      std::function<void (int fd)> on_event;
    private:
      fd_set read_set;
      int    max_fd;
    };
public:
    QPacketSniffer(const QString& interface_name, QObject* parent);
    ~QPacketSniffer();

    void run();
    void wait();
private:
    void    initialize_sock_address();
    void    initialize_protocol();

    int                  sock;
    struct sockaddr_ndrv sock_address;
};

#endif
