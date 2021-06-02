PROGRAM = unace

OBJ = globals.o uac.o uac_crt.o uac_dcpr.o unace.o
SRC = globals.c uac.c uac_crt.c uac_dcpr.c unace.c

#CFLAGS = -g -Wall -DMDEBUG
#CFLAGS = -O3 -fno-strength-reduce -fomit-frame-pointer # These are for Linux

CFLAGS = -O -g -Wall
CC = gcc

all: $(PROGRAM)

$(PROGRAM): $(OBJ) $(LIBS)
	$(CC) $(CFLAGS) $(OBJ) -o $(PROGRAM) $(LIBS)

clean:
	rm -f *.o a.out core $(PROGRAM) *.exe
	rm -f makefile.dep

makefile.dep: $(SRC)
	$(CPP) $(CPPFLAGS) -MM $(SRC) >$@

#======================================================================
# make makefile.dep
globals.o: globals.c acestruc.h declare.h pendian_detect.h unace.h
uac.o: uac.c globals.h acestruc.h declare.h pendian_detect.h unace.h \
 uac.h
uac_crt.o: uac_crt.c declare.h pendian_detect.h globals.h acestruc.h \
 unace.h uac.h
uac_dcpr.o: uac_dcpr.c declare.h pendian_detect.h globals.h acestruc.h \
 unace.h uac.h
unace.o: unace.c declare.h pendian_detect.h globals.h acestruc.h unace.h \
 uac.h
#======================================================================
