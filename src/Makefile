
include ../config.mk

APP  = unace1
# ls *.c | sed 's%\.c%.o%g'
OBJS = uac.o uac_dcpr.o unace.o

#CPPFLAGS += -I..
CFLAGS += -I.. -I../lib

all: $(APP)

$(APP): $(OBJS)
	@if ! test -f $(APP) && ! test -f $(APP).exe ; then \
		echo "$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS)"; \
		$(CC) $(CFLAGS) $(OBJS) -o $(APP) $(LDFLAGS) $(LIBS); \
	fi;

strip: $(APP)
	@if test -f $(APP).exe ; then \
		echo $(STRIP) $(APP).exe; \
		$(STRIP) $(APP).exe; \
	else \
		echo $(STRIP) $(APP); \
		$(STRIP) $(APP); \
	fi;

distclean: clean

clean:
	rm -f *.o *~ $(APP) $(APP).exe

install: all
	mkdir -p $(DESTDIR)$(bindir)
	@if test -f $(APP).exe ; then \
		echo install $(APP).exe; \
		install $(APP).exe $(DESTDIR)$(bindir); \
	else \
		echo install $(APP); \
		install $(APP) $(DESTDIR)$(bindir); \
	fi;

install-strip: strip install

uninstall:
	rm -f $(DESTDIR)$(bindir)/$(APP)

