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

#CFLAGS = -DNOSTRICMP         #for UNIX compilers not supporting stricmp()
#CFLAGS = -DHI_LO_BYTE_ORDER  #select correct byte order for your machine!!

#CFLAGS = -Wall -O3 -s #-DNDEBUG
#CFLAGS = -g -Wall -DMDEBUG
#CFLAGS = -O3 -fno-strength-reduce -fomit-frame-pointer # These are for Linux
CFLAGS = -O -g -Wall
CHALLOC = challoc.o
CHALLOC =
CHNEW = chnew.o challoc.o
CC = gcc

OSTYPE = $(shell uname -s)

ifeq ($(OSTYPE), Linux)
CPPFLAGS += -DUNIX -DCASEINSENSE
else
ifeq ($(OSTYPE), DOS)
EXEEXT = .exe
CPPFLAGS += -DDOS
else
CPPFLAGS += -DUNIX -DCASEINSENSE
endif
endif

# not all systems require the math library
#LIBS = -lm


unace$(EXEEXT): $(OBJ) $(CHALLOC) $(LIBS)

clean:
	rm -f *.o a.out core unace unace.exe
	rm -f makefile.dep

makefile.dep: $(SRC)
	$(CPP) $(CPPFLAGS) -MM $(SRC) >$@

-include makefile.dep
