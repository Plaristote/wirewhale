#include "wirewhale.h"
#include "networkinterfacelist.h"
#include "filterprofile.h"
#include <iostream>
using namespace std;
Wirewhale::Wirewhale(QObject *parent) : QObject(parent)
{
  packetTable    = new QPacketTable(this);
  packetListener = new QPacketListener(packetTable, this);
}

QStringList Wirewhale::getProfileList() const
{
  return FilterProfile::filterList();
}

QStringList Wirewhale::getInterfaceList() const
{
  NetworkInterfaceList interfaceList;

  return interfaceList;
}

void Wirewhale::setQmlContext(QQmlContext* context)
{
  context->setContextProperty("packetLogModel", packetTable);
  context->setContextProperty("packetListener", packetListener);
}
