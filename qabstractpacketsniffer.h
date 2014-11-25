#ifndef QABSTRACTPACKETSNIFFER_H
#define QABSTRACTPACKETSNIFFER_H

#include <QObject>

class QAbstractPacketSniffer : public QObject
{
    Q_OBJECT
public:
    explicit QAbstractPacketSniffer(QObject *parent = 0);

signals:

public slots:

};

#endif // QABSTRACTPACKETSNIFFER_H
