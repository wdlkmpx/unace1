
include ../config.mk

APP  = unace1$(EXE)

# ls *.c | sed 's%\.c%.o%g'
OBJS = uac.o uac_dcpr.o unace.o

#CPPFLAGS += -I..
CFLAGS += -I.. -I../lib

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)

strip: $(APP)
	$(STRIP) $(APP)

distclean: clean

clean:
	rm -f *.o *~ $(APP)

install: all
	install $(APP) $(DESTDIR)$(bindir)

install-strip: strip install

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(APP)
