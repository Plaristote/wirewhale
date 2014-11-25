#ifndef  OSX_QPACKET_SNIFFER_H
# define OSX_QPACKET_SNIFFER_H

# include "qabstractpacketsniffer.h"
# include <sys/socket.h>

class QPacketSniffer : public QAbstractPacketSniffer
{
    Q_OBJECT

public:
    QPacketSniffer(const QString& interface_name, QObject* parent);
    ~QPacketSniffer();

    void run();
private:
    int     sock;
};

#endif
