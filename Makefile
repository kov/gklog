#SHELL = /bin/bash
CFLAGS = -O
#-DDEBUG

# Edited for Debian GNU/Linux.
DESTDIR =


BIN = $(DESTDIR)/usr/bin

all:	
	gcc $(CFLAGS) -c gklog.c
	gcc $(CFLAGS) -c read-conf.c
	gcc $(CFLAGS) -o gklog gklog.o read-conf.o


gklog: all


bindir=/opt/gklog/bin
etcdir=/opt/gklog/etc
mandir=/opt/gklog/man/man1
install:
	test -d $(bindir) || mkdirhier $(bindir)
	bsdinst -c  -s gklog $(bindir)
	bsdinst -c  -m 0755 gklog.sh $(bindir)
	test -d $(etcdir) || mkdirhier $(etcdir)
	bsdinst -c  -m 0644 gklog.rules $(etcdir)
	test -d $(etcdir)/init.d || mkdirhier $(etcdir)/init.d
	test -d $(etcdir)/rc2.d || mkdirhier $(etcdir)/rc2.d
	test -d $(mandir) || mkdirhier $(mandir)
	bsdinst -c  -m 0644 gklog.1 $(mandir)

old_install: 
	install -d $(DESTDIR)/etc/init.d/
	install -d $(DESTDIR)/etc/rc2.d/
	install -d $(DESTDIR)/usr/bin/
	install -d $(DESTDIR)/usr/man/man1/
	install -m 664 gklog.1 $(DESTDIR)/usr/man/man1/
	install -m 664 gklog.rules $(DESTDIR)/etc/
	install gklog $(BIN)

uninstall:
	-rm -f $(DESTDIR)/etc/init.d/gklog.sh
	-rm -f $(DESTDIR)/etc/rc2.d/S90gklog
	-rm -f $(DESTDIR)/usr/bin/gklog
	-rm -f $(DESTDIR)/etc/gklog.rules
	-rm -f $(DESTDIR)/usr/man/man1/gklog.1

clean:
	-rm -f gklog
	-rm -f *.o
