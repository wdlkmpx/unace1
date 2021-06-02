# http://unlicense.org/

MAKEFLAGS += --no-print-directory

SUBDIRS = src

all: subdirs

subdirs:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

distclean:
#	rm -f Makefile config.h
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir distclean; \
	done

install:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir install; \
	done

uninstall:
	@for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir uninstall; \
	done


.PHONY: subdirs $(SUBDIRS)
