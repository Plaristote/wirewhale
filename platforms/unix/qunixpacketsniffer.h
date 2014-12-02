#ifndef QUNIXPACKETSNIFFER_H
# define QUNIXPACKETSNIFFER_H

# include "qabstractpacketsniffer.h"

class QUnixPacketSniffer : public QAbstractPacketSniffer
{
    Q_OBJECT
protected:
    explicit QUnixPacketSniffer(const QString& interface_name, QObject *parent = 0);

    void capture_packet();
    void append_packet(const Packet&);

    int sock;
};

#endif // QUNIXPACKETSNIFFER_H
