#ifndef  LINUX_QPACKET_SNIFFER_H
# define LINUX_QPACKET_SNIFFER_H

# include <sys/socket.h>

class QPacketSniffer : public QAbstractPacketSniffer
{
    Q_OBJECT

public:
    QPacketSniffer(const QString& interface_name, QObject* parent = 0);
    ~QPacketSniffer();

private:
    void initialize_interface();
    void initialize_sock_address();

    QString            interface_name;
    int                sock;
    struct ifreq       interface;
    struct sockaddr_ll sock_address;
};

#endif
