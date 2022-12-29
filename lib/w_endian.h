// Public Domain

/*
 https://gist.github.com/PkmX/63dd23f28ba885be53a5
 https://gist.github.com/panzi/6856583
 
 docs:
   https://man7.org/linux/man-pages/man3/endian.3.html
   https://man7.org/linux/man-pages/man3/bswap.3.html
   https://man.openbsd.org/OpenBSD-5.6/byteorder.3

 Defines WORDS_BIGENDIAN if a big endian system is detected
   (compatibility with autoconf)
*/
 
#ifndef __W_ENDIAN_H__
#define __W_ENDIAN_H__

#ifdef __cplusplus
extern "C" {
#endif

//#define DEBUG_W_ENDIAN

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// ========================================================
// known systems

#if defined(__linux__) || defined(__HAIKU__) || defined(__CYGWIN__) || defined(__GNU__)
#   include <endian.h>
#   include <byteswap.h>

#elif defined(__OpenBSD__) 
#   include <endian.h> /* OpenBSD >= 5.6 */

#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
#   include <sys/endian.h>

#elif defined(sun) || defined(__sun)
#  include <sys/byteorder.h>
#   define bswap16(x) BSWAP_16(x)
#   define bswap32(x) BSWAP_32(x)
#   define bswap64(x) BSWAP_64(x)

#elif defined(__APPLE__)
#   include <libkern/OSByteOrder.h>
#   define bswap16(x) OSSwapInt16(x)
#   define bswap32(x) OSSwapInt32(x)
#   define bswap64(x) OSSwapInt64(x)

#elif defined(_WIN32)
//#   include <windows.h>
#   if defined(_MSC_VER)
#       include <stdlib.h>
#       define bswap16(x) _byteswap_ushort(x)
#       define bswap32(x) _byteswap_ulong(x)
#       define bswap64(x) _byteswap_uint64(x)
#   elif defined(__GNUC__) || defined(__clang__)
#       define bswap16(x) __builtin_bswap16(x)
#       define bswap32(x) __builtin_bswap32(x)
#       define bswap64(x) __builtin_bswap64(x)
#   endif
#endif


// ========================================================
// may need to define custom bswapXX functions

#if !defined(bswap16)
#   if defined(bswap_16)
#       define bswap16(x) bswap_16(x)
#       define bswap32(x) bswap_32(x)
#       define bswap64(x) bswap_64(x)
#   endif
#endif

// also support bswap_XX() (only the GNU Libc defines them)
#if !defined(bswap_16) && !defined(bswap_32)
#   define bswap_16(x) bswap16(x)
#   define bswap_32(x) bswap32(x)
#   define bswap_64(x) bswap64(x)
#endif

#if !defined(bswap16) || !defined(bswap32) || !defined(bswap64)
#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG] CUSTOM swapXX functions
#   endif

#   include <inttypes.h>
#   define bswap16(x) _bswap16(x)
#   define bswap32(x) _bswap32(x)
#   define bswap64(x) _bswap64(x)

    static inline uint16_t _bswap16(uint16_t x)
    {
        return ((x << 8) & 0xff00) | ((x >> 8) & 0x00ff);
    }

    static inline uint32_t _bswap32(uint32_t x)
    {
        return ((x << 24) & 0xff000000 ) |
               ((x <<  8) & 0x00ff0000 ) |
               ((x >>  8) & 0x0000ff00 ) |
               ((x >> 24) & 0x000000ff );
    }

    static inline uint64_t _bswap64(uint64_t x)
    {
        uint32_t l32, h32;
        h32 = _bswap32((uint32_t)(x & 0x00000000ffffffffULL));
        l32 = _bswap32((uint32_t)((x >> 32) & 0x00000000ffffffffULL));
        return ((uint64_t)h32 << 32) | l32;
    }
#endif


// ========================================================
// define W_LITTLE_ENDIAN or W_BIG_ENDIAN

#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && defined(__ORDER_LITTLE_ENDIAN__)
#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG]  using __BYTE_ORDER__ to determine endianness
#   endif
#   if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#       define W_LITTLE_ENDIAN 1
#   elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#       define W_BIG_ENDIAN 1
#   endif

#elif defined(__BYTE_ORDER) && defined(__ORDER_BIG_ENDIAN) && defined(__ORDER_LITTLE_ENDIAN)
#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG]  using __BYTE_ORDER (1) to determine endianness
#   endif
#   if __BYTE_ORDER == __ORDER_LITTLE_ENDIAN
#       define W_LITTLE_ENDIAN 1
#   elif __BYTE_ORDER == __ORDER_BIG_ENDIAN
#       define W_BIG_ENDIAN 1
#   endif

#elif defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && defined(__LITTLE_ENDIAN)
#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG] using __BYTE_ORDER (2) to determine endianness
#   endif
#   if __BYTE_ORDER == __BIG_ENDIAN
#       define W_LITTLE_ENDIAN 1
#   elif __BYTE_ORDER == __LITTLE_ENDIAN
#       define W_BIG_ENDIAN 1
#   endif

#elif defined(_BYTE_ORDER) && defined(_BIG_ENDIAN) && defined(_LITTLE_ENDIAN)
    //solaris is known to use this
#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG] using _BYTE_ORDER to determine endianness
#   endif
#   if _BYTE_ORDER == _BIG_ENDIAN
#       define W_LITTLE_ENDIAN 1
#   elif _BYTE_ORDER == _LITTLE_ENDIAN
#       define W_BIG_ENDIAN 1
#   endif

#elif defined(BYTE_ORDER) && defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN)
    //FreeBSD and similir os'es
#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG] using BYTE_ORDER to determine endianness
#   endif
#   if BYTE_ORDER == BIG_ENDIAN
#       define W_LITTLE_ENDIAN 1
#   elif BYTE_ORDER == LITTLE_ENDIAN
#       define W_BIG_ENDIAN 1
#   endif

#elif defined(__LITTLE_ENDIAN__) ||                                      \
      defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
      defined(_MIPSEL)  || defined(__MIPSEL)   || defined(__MIPSEL__)      || \
      defined(_M_X64)   || defined(_M_IX86)    || defined(_M_I86)          || \
      defined(__i386__) || defined(sun386)     || defined(__alpha__)  ||   \
      defined(__ia64)   || defined(__ia64__)   ||                          \
      defined(_M_IA64)  || defined(_M_ALPHA)   ||                          \
      defined(__amd64)  || defined(__amd64__)  || defined(_M_AMD64)   ||   \
      defined(__x86_64) || defined(__x86_64__) || defined(_M_X64)     ||   \
      defined(__bfin__) || defined(__VMS)
#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG] extra LITTLE ENDIAN macros detected
#   endif
#   define W_LITTLE_ENDIAN 1

#elif defined (__BIG_ENDIAN__) ||                                              \
      defined(__ARMEB__) || defined(THUMBEB__)   || defined (__AARCH64EB__) || \
      defined(_MIPSEB)   || defined(__MIPSEB)    || defined(__MIPSEB__)     || \
      defined(__sparc)   || defined(__sparc__)   ||                            \
      defined(_POWER)    || defined(__powerpc__) || defined(__ppc__)        || \
      defined(__hpux)    || defined(__hppa)      ||                            \
      defined(__s390__)  || defined(__s390x__)   || defined(__zarch__)
#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG] extra BIG ENDIAN macros detected
#   endif
#   define W_BIG_ENDIAN 1
#endif

#if defined(W_LITTLE_ENDIAN) && defined(W_BIG_ENDIAN)
#error something went wrong - W_LITTLE_ENDIAN & W_BIG_ENDIAN are defined
#endif

#if !defined(W_LITTLE_ENDIAN) && !defined(W_BIG_ENDIAN)
#warning cannot identify endianness, assuming LITTLE ENDIAN...
#define W_LITTLE_ENDIAN 1
#endif

#ifdef W_LITTLE_ENDIAN
#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG] LITTLE ENDIAN detected
#   endif
#   ifndef __LITTLE_ENDIAN__
#       define __LITTLE_ENDIAN__ 1
#   endif
#endif

#ifdef W_BIG_ENDIAN
#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG] BIG ENDIAN detected
#   endif
#   ifndef __BIG_ENDIAN__
#       define WORDS_BIGENDIAN 1
#       define __BIG_ENDIAN__ 1
#   endif
#endif


// ========================================================
// the system may not define the BSD endianness conversion macross

#if !defined(be16toh) && !defined(betoh16)

#   ifdef DEBUG_W_ENDIAN
#       warning [DEBUG] USING CUSTOM ENDIANNESS CONVERSION DEFINES
#   endif

#   ifdef W_LITTLE_ENDIAN
#       define htobe16(x) bswap16(x)
#       define htole16(x) (x)
#       define be16toh(x) bswap16(x)
#       define le16toh(x) (x)

#       define htobe32(x) bswap32(x)
#       define htole32(x) (x)
#       define be32toh(x) bswap32(x)
#       define le32toh(x) (x)

#       define htobe64(x) bswap64(x)
#       define htole64(x) (x)
#       define be64toh(x) bswap64(x)
#       define le64toh(x) (x)

#   else /* BIG ENDIAN */
#       define htobe16(x) (x)
#       define htole16(x) bswap16(x)
#       define be16toh(x) (x)
#       define le16toh(x) bswap16(x)

#       define htobe32(x) (x)
#       define htole32(x) bswap32(x)
#       define be32toh(x) (x)
#       define le32toh(x) bswap32(x)

#       define htobe64(x) (x)
#       define htole64(x) bswap64(x)
#       define be64toh(x) (x)
#       define le64toh(x) bswap64(x)
#   endif

#endif

// OpenBSD introduced betoh16 but it's not used by other
//   UNIX-like systems (they use be16toh instead)
#if defined(betoh16) && !defined(be16toh)
#   define be16toh(x) betoh16(x)
#   define le16toh(x) letoh16(x)
#   define be32toh(x) betoh32(x)
#   define le32toh(x) letoh32(x)
#   define be64toh(x) betoh64(x)
#   define le64toh(x) letoh64(x)
#elif !defined(betoh16) && defined(be16toh)
#   define betoh16(x) be16toh(x)
#   define letoh16(x) le16toh(x)
#   define betoh32(x) be32toh(x)
#   define letoh32(x) le32toh(x)
#   define betoh64(x) be64toh(x)
#   define letoh64(x) le64toh(x)
#endif

// ========================================================

#ifdef __cplusplus
}
#endif

#endif /* __W_ENDIAN_H__ */

