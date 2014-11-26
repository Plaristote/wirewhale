#ifndef  OSX_QPACKET_SNIFFER_H
# define OSX_QPACKET_SNIFFER_H

# include "qabstractpacketsniffer.h"
# include <sys/socket.h>
# include <net/if.h>
# include <net/ndrv.h>

class QPacketSniffer : public QAbstractPacketSniffer
{
    Q_OBJECT

public:
    QPacketSniffer(const QString& interface_name, QObject* parent);
    ~QPacketSniffer();

    void run();
    void wait();
private:
    void    initialize_sock_address();

    int     sock;
    struct sockaddr_ndrv sock_address;
};

#endif
