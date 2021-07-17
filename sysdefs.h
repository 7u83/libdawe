#ifndef __DEF_H
#define __DEF_H

#if defined(__STDC__)
# define DAWE_CVERSION 89L
# if defined(__STDC_VERSION__)
#  undef DAWE_CVERSION
#  define DAWE_CVERSION 90L
#  if (__STDC_VERSION__ >= 199409L)
#   undef DAWE_CVERSION
#   define DAWE_CVERSION 94L
#  endif
#  if (__STDC_VERSION__ >= 199901L)
#   undef DAWE_CVERSION
#   define DAWE_CVERSION 99L
#  endif
#  if (__STDC_VERSION__ >= 201112L)
#   undef DAWE_CVERSION
#   define DAWE_CVERSION 11L
#  endif
# endif
#endif


#if (DAWE_CVERSION >= 99L)
  #include <stdint.h>
#else
  typedef unsigned char uint8_t;
  typedef unsigned short uint16_t;
  typedef unsigned int uint32_t;
  #define inline
#endif

#include <arpa/inet.h>

#ifndef htole32
  #define htole32(X) (htonl(htonl(X)))
#endif

#ifndef letoh32
  #define letoh32(X) (ntohl(ntohl(X)))
#endif


#ifndef htole16
  #define htole16(X) (htons(htons(X)))
#endif

#endif

