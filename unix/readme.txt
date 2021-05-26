To compile under unix:

1) Copy the contents of this directory to the source files
2) Check the makefile and select a suitable set of CFLAGS
3) 'make dep'
4) 'make'

If the resulting 'unace' can't process any archives it might be
because the machine you are using isn't using the same byte-order
as Intel CPUs. In that case you need to replace

#if defined(UNIX)
  #define DIRSEP '/'
  #define LO_HI_BYTE_ORDER
#endif

in os.h with

#if defined(UNIX)
  #define DIRSEP '/'
  #define HI_LO_BYTE_ORDER
#endif

Good luck.

/Mikael Nordqvist
