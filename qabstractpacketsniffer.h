#ifndef QABSTRACTPACKETSNIFFER_H
#define QABSTRACTPACKETSNIFFER_H

#include <QObject>

class QAbstractPacketSniffer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString interface_name READ getInterfaceName)
public:
    explicit QAbstractPacketSniffer(const QString& interface_name, QObject *parent = 0);

    const QString& getInterfaceName(void) { return (interface_name); }

    virtual void run(void) = 0;

signals:

public slots:

protected:
    QString interface_name;
};

#endif // QABSTRACTPACKETSNIFFER_H
