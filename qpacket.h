#ifndef QPACKET_H
# define QPACKET_H

# include <QString>
# include <QDateTime>

struct QPacket
{
public:
  QPacket();

  enum EthernetType
  {
    IPv4,
    IPv6,
    ARP
  };

  bool         operator==(long p) const { return (p == number); }

  QDateTime    dateTime()
  {
    QDateTime datetime;

    datetime.setTime_t(time);
    return datetime;
  }

  EthernetType ethernet_type;
  short        alertLevel;
  long         number;
  long         time;
  QString      source;
  QString      destination;
  QString      protocol;
  long         length;
  QString      information;
  QString      payload;
};

#endif // QPACKET_H
