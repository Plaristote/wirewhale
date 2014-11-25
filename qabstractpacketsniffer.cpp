#include "qabstractpacketsniffer.h"

QAbstractPacketSniffer::QAbstractPacketSniffer(const QString& interface_name, QObject *parent) :
    QObject(parent), interface_name(interface_name)
{
}
