#include "networkinterfacelist.h"
#include <string>
#include <set>
#include <QNetworkInterface>

using namespace std;

NetworkInterfaceList::NetworkInterfaceList(QObject *parent) : QStringList()
{
  appendAllInterfaces();
}

void NetworkInterfaceList::appendAllInterfaces()
{
  foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces())
  {
    append(interface.name());
  }
}
