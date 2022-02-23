
include ../config.mk

APP  = unace1
# ls *.c | sed 's%\.c%.o%g'
OBJS = globals.o uac.o uac_dcpr.o unace.o
SRCS = globals.c uac.c uac_dcpr.c unace.c

#CPPFLAGS += -I..
CFLAGS += -I..

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: $(APP)
	$(STRIP) $(APP)

distclean: clean

clean:
	rm -f *.o *~ $(APP) $(APP).exe

install: $(APP)
	mkdir -p $(DESTDIR)$(bindir)
	install $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(APP)

makefile.dep:
	$(CC) -MM $(SRCS) > makefile.dep

# ------------------------------------------------------------------------
# make makefile.dep
globals.o: globals.c acestruc.h declare.h pendian_detect.h unace.h
uac.o: uac.c globals.h acestruc.h declare.h pendian_detect.h unace.h \
 uac.h
uac_dcpr.o: uac_dcpr.c declare.h pendian_detect.h globals.h acestruc.h \
 unace.h uac.h
unace.o: unace.c declare.h pendian_detect.h globals.h acestruc.h unace.h \
 uac.h
