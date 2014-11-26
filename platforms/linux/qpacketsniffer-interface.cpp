#include "qpacketsniffer.h"
#include <unistd.h>
#include <linux/sockios.h>
#include <linux/if_ether.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stropts.h>
#include <string.h>
#include <iostream>
#include <errno.h>

QPacketSniffer::Interface::Interface(const QString &name)
{
  int name_length = name.length() > IFNAMSIZ - 1 ? IFNAMSIZ - 1 : name.length();

  memset(&data, 0, sizeof(data));
  strncpy(data.ifr_name, name.toUtf8().data(), name_length);
}

QPacketSniffer::Interface::~Interface()
{
}

void QPacketSniffer::Interface::initialize_if_index(int fd)
{
  if ((ioctl(fd, SIOGIFINDEX, &data)) == -1)
    raise("cannot initialize interface");
}

void QPacketSniffer::Interface::enable_promiscuous_mode(int fd)
{
  if (!(data.ifr_flags & IFF_PROMISC))
  {
    data.ifr_flags |= IFF_PROMISC;
    if ((ioctl(fd, SIOCSIFFLAGS, &data)) == -1)
      raise("cannot enable promiscuous mode");
  }
}

void QPacketSniffer::Interface::disable_promiscuous_mode(int fd)
{
  if (data.ifr_flags & IFF_PROMISC)
  {
    data.ifr_flags &= ~IFF_PROMISC;
    if ((ioctl(fd, SIOCSIFFLAGS, &data)) == -1)
      raise("cannot disable promiscuous mode");
  }
}

void QPacketSniffer::Interface::raise(const QString &message)
{
    QString full_message = message + " for interface '" + QString(data.ifr_name) + "': " + QString(strerror(errno));

    throw std::runtime_error(full_message.toUtf8().data());
}

