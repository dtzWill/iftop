#
# Makefile:
# Makefile for iftop.
#
# $Id$
#

# C compiler to use.
#CC = gcc

# Give the location of pcap.h here:
CFLAGS += -I/usr/include/pcap

# Give the location of libpcap here if it's not in one of the standard
# directories:
#LDFLAGS += -L/usr/local/lib

# PREFIX specifies the base directory for the installation.
#PREFIX = /usr/local
PREFIX = /software

# BINDIR is where the binary lives. No leading /.
BINDIR = sbin

# MANDIR is where the manual page goes.
MANDIR = man
#MANDIR = share/man     # FHS-ish

# You shouldn't need to change anything below this point.
CFLAGS  += -g -Wall 
LDFLAGS += -g 
LDLIBS += -lpcap -lpthread -lcurses -lm

SRCS =  iftop.c \
        addr_hash.c \
	hash.c \
	ns_hash.c \
	resolver.c \
	ui.c \
        util.c \
	sorted_list.c

OBJS = $(SRCS:.c=.o)

HDRS =  addr_hash.h	

# If you do not have makedepend, you will need to remove references to depend
# and nodepend below.
iftop: depend $(OBJS) Makefile
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS) 

install: iftop
	install -D iftop   $(PREFIX)/$(BINDIR)
	install -D iftop.8 $(PREFIX)/$(MANDIR)/man8

uninstall:
	rm -f $(PREFIX)/$(BINDIR)/iftop $(PREFIX)/$(MANDIR)/man8/iftop.8

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

clean: nodepend
	rm -f *~ *.o core iftop

tags :
	etags *.c *.h

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)
	touch depend

nodepend:
	makedepend -- --
	rm -f depend
 
# DO NOT DELETE
