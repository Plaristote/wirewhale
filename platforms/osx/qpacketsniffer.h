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
public:
    QPacketSniffer(const QString& interface_name, QObject* parent);
    ~QPacketSniffer();

    void run();
    void wait();
    void stop();
private:
    void    initialize_sock_address();
    void    initialize_protocol();

    int                  sock;
    struct sockaddr_ndrv sock_address;
    Poll                 poll;
    bool                 must_stop;
    QMutex               mutex;
    Qt::HANDLE           sniffing_thread;
};

#endif
