#ifndef NETWORKINTERFACELIST_H
#define NETWORKINTERFACELIST_H

#include <QStringList>

class NetworkInterfaceList : public QStringList
{
public:
    explicit NetworkInterfaceList(QObject *parent = 0);

private:
    void appendAllInterfaces();
};

#endif // NETWORKINTERFACELIST_H
