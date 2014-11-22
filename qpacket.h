#ifndef QPACKET_H
# define QPACKET_H

# include <QString>

struct QPacket
{
public:
  QPacket();

  bool     operator==(long p) const { return (p == number); }
  short    alertLevel;
  long     number;
  long     time;
  QString  source;
  QString  destination;
  QString  protocol;
  long     length;
  QString  information;
};

#endif // QPACKET_H
