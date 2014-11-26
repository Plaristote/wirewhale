#ifndef QPACKETSNIFFER_H
# define QPACKETSNIFFER_H

# include "qabstractpacketsniffer.h"

# if defined(__MACH__)
#  include "platforms/osx/qpacketsniffer.h"
# elif defined(__linux__)
#  include "platforms/linux/qpacketsniffer.h"
# elif defined(WIN32)
#  include "platforms/windows/qpacketsniffer.h"
# endif

#endif // QPACKETSNIFFER_H
