# The unregistered unace GNU makefile
# if you don't have GNU make or bash, then you should proly
# remove the ifeq <-> endif statments, and configure CFLAGS yourself
#
# orig - chad hurwitz 3-11-98 churritz@cts.com
#
# -DCASEINSENSE is for people with case insensitive filesystems
#

OBJ = globals.o uac_comm.o uac_crc.o uac_crt.o uac_dcpr.o uac_sys.o unace.o
SRC = globals.c uac_comm.c uac_crc.c uac_crt.c uac_dcpr.c uac_sys.c unace.c

#CFLAGS = -dNOSTRICMP         #for UNIX compilers not supporting stricmp()
#CFLAGS = -dHI_LO_BYTE_ORDER  #select correct byte order for your machine!!

#CFLAGS = -Wall -O3 -s #-DNDEBUG
#CFLAGS = -g -Wall -DMDEBUG
#CFLAGS = -O3 -fno-strength-reduce -fomit-frame-pointer # These are for Linux
CFLAGS = -O -g -Wall -dLO_HI_BYTE_ORDER
CHALLOC = challoc.o
CHALLOC =
CHNEW = chnew.o challoc.o
CC = gcc

ifeq ($(OSTYPE), Linux)
CFLAGS += -DUNIX -DCASEINSENSE
else
ifeq ($(OSTYPE), DOS)
EXEEXT = .exe
CFLAGS += -DDOS
else
CFLAGS += -DUNIX -DCASEINSENSE
endif
endif

# not all systems require the math library
#LIBS = -lm


unace$(EXEEXT): $(OBJ) $(CHALLOC)
	gcc $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f *.o a.out core unace unace.exe

dep:
	chmod a+rx ./gccmaked
	./gccmaked -- $(CFLAGS) -- *.c*
	rm -f ?akefile.bak

# DO NOT DELETE
globals.o: globals.c os.h acestruc.h declare.h portable.h unace.h
uac_comm.o: uac_comm.c globals.h acestruc.h declare.h portable.h os.h \
 unace.h uac_dcpr.h uac_comm.h
uac_crc.o: uac_crc.c uac_crc.h declare.h
uac_crt.o: uac_crt.c os.h globals.h acestruc.h declare.h portable.h \
 unace.h uac_crt.h uac_sys.h
uac_dcpr.o: uac_dcpr.c os.h globals.h acestruc.h declare.h portable.h \
 unace.h uac_comm.h uac_crc.h uac_dcpr.h uac_sys.h
uac_sys.o: uac_sys.c os.h globals.h acestruc.h declare.h portable.h \
 unace.h uac_sys.h
unace.o: unace.c os.h globals.h acestruc.h declare.h portable.h \
 unace.h uac_comm.h uac_crc.h uac_crt.h uac_dcpr.h uac_sys.h
