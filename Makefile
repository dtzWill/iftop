#
# Makefile:
# Makefile for iftop.
#
# $Id: Makefile,v 1.25 2002/08/26 09:44:46 pdw Exp $
#

# C compiler to use.
#CC = gcc

# Give the location of pcap.h here:
CFLAGS += -I/usr/include/pcap

# Give the location of libpcap here if it's not in one of the standard
# directories:
#LDFLAGS += -L/usr/local/lib

# PREFIX specifies the base directory for the installation.
PREFIX = /usr/local
#PREFIX = /software

# BINDIR is where the binary lives. No leading /.
BINDIR = sbin

# MANDIR is where the manual page goes.
MANDIR = man
#MANDIR = share/man     # FHS-ish

# You shouldn't need to change anything below this point.
VERSION = 0.6
CFLAGS  += -g -Wall "-DIFTOP_VERSION=\"$(VERSION)\""
LDFLAGS += -g 
LDLIBS += -lpcap -lpthread -lcurses -lm

SRCS = iftop.c addr_hash.c hash.c ns_hash.c resolver.c ui.c util.c sorted_list.c\
       options.c
HDRS = addr_hash.h hash.h iftop.h ns_hash.h resolver.h sorted_list.h ui.h options.h
TXTS = README CHANGES INSTALL TODO iftop.8 COPYING
SPECFILE = iftop.spec

OBJS = $(SRCS:.c=.o)

iftop: $(OBJS) Makefile
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS) 

install: iftop
	install -D iftop   $(PREFIX)/$(BINDIR)/iftop
	install -D iftop.8 $(PREFIX)/$(MANDIR)/man8/iftop.8

uninstall:
	rm -f $(PREFIX)/$(BINDIR)/iftop $(PREFIX)/$(MANDIR)/man8/iftop.8

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *~ *.o core iftop

tarball: depend $(SRCS) $(HDRS) $(TXTS) $(SPECFILE)
	mkdir iftop-$(VERSION)
	set -e ; for i in Makefile depend $(SRCS) $(HDRS) $(TXTS) $(SPECFILE) ; do cp $$i iftop-$(VERSION)/$$i ; done
	tar cvf - iftop-$(VERSION) | gzip --best > iftop-$(VERSION).tar.gz
	rm -rf iftop-$(VERSION)

tags :
	etags *.c *.h

depend: $(SRCS)
	$(CPP) $(CFLAGS)  -MM $(SRCS) > depend

nodepend:
	rm -f depend
        
include depend
