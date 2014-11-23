#include "networkinterfacelist.h"
#include <tins/tins.h>
#include <string>
#include <set>

using namespace std;

NetworkInterfaceList::NetworkInterfaceList(QObject *parent) : QStringList()
{
  appendAllInterfaces();
}

void NetworkInterfaceList::appendAllInterfaces()
{
  const set<std::string> interfaces = Tins::Utils::network_interfaces();

  for_each(interfaces.begin(), interfaces.end(), [this](string name)
  {
    this->append(QString::fromStdString(name));
  });
}
