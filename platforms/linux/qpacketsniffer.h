#ifndef  LINUX_QPACKET_SNIFFER_H
# define LINUX_QPACKET_SNIFFER_H

# include "qabstractpacketsniffer.h"
# include <sys/socket.h>

class QPacketSniffer : public QAbstractPacketSniffer
{
    Q_OBJECT

public:
    QPacketSniffer(const QString& interface_name, QObject* parent = 0);
    ~QPacketSniffer();

    void run();

private:
    void initialize_interface();
    void initialize_sock_address();
    void capture_packet();

    static size_t packet_offset_ip_header();
    static size_t packet_offset_xcp_header();

    QString            interface_name;
    int                sock;
    struct ifreq       interface;
    struct sockaddr_ll sock_address;
};

#endif
