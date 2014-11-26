#include "qpacketsniffer.h"

QPacketSniffer::QPacketSniffer(const QString& interface_name, QObject* parent) :
    QAbstractPacketSniffer(interface_name, parent)
{
}

QPacketSniffer::~QPacketSniffer()
{
}

void QPacketSniffer::run()
{
}

void QPacketSniffer::stop()
{
}

void QPacketSniffer::wait()
{
}
