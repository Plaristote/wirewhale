#ifndef  WIREWHALE_ENDIANNESS_H
# define WIREWHALE_ENDIANNESS_H

# include <stdint.h>

# if defined(__APPLE__)
#  include <sys/types.h>
#  define USE_LITTLE_ENDIAN (BYTE_ORDER == LITTLE_ENDIAN)
# elif defined(BSD)
#  include <sys/types.h>
#  define USE_LITTLE_ENDIAN (_BYTE_ORDER == _LITTLE_ENDIAN)
# else
#  include <endian.h>
#  define USE_LITTLE_ENDIAN (__BYTE_ORDER == __LITTLE_ENDIAN)
# endif

#endif // ENDIANNESS_H
