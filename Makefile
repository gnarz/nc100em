# makefile for nc100em

# You need an ANSI C compiler. gcc is probably best.
#
CC=gcc

# set this if compiling xnc100em; it should point to where X is
# installed. If you don't know where it is, try `/usr/X11', that's a
# common culprit. This is where it is on my Linux box, and probably on
# most other XFree86-using systems:
#
XROOT=/usr

# There are various compile-time options. Generally you won't want to
# change the `CFLAGS' line below, but they're described below in case
# you do.
#
# -DSCALE=n sets how many times to scale up the window, 1=normal.
#  In xnc100em, this isn't changeable after compiling, and doesn't
#  work on 1-bit displays. In gnc100em, it can be changed at run-time
#  and should work on any display (for this version, SCALE only sets
#  the default initial scale, which can be overridden with `-S').
#
# -DUSE_MMAP_FOR_CARD enables use of mmap() to access `nc100.card',
#  the PCMCIA memory card image. The main effect of this is that
#  the emulator starts/stops rather more quickly, and only
#  bits of the card which are accessed are read/written. It also
#  makes zcntools *much* faster, and means you can use zcntools while
#  an emulator is running (if you're careful) because of ZCN's
#  stateless `disk' I/O, despite what it says in the man page. :-) So
#  you probably shouldn't remove it unless you have problems compiling.
#
# -DMITSHM is for the Xlib version, and tells it to use shared memory
#  if the X server is local (running on the same machine) and supports
#  it. Don't remove it unless you have problems compiling.
#
# As I say, the default settings below should generally be ok.
#
#CFLAGS=-DMITSHM -DUSE_MMAP_FOR_CARD -DSCALE=1 -O -Wall -I$(XROOT)/include
CFLAGS=-DMITSHM -DUSE_MMAP_FOR_CARD -DSCALE=1 -g -Wall -I$(XROOT)/include

# dest for make install
#
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
MANDIR=$(PREFIX)/man/man1

# you shouldn't need to edit the rest
#-----------------------------------------------------------------

# this looks wrong, but *ops.c are actually #include'd by z80.c
DNC100EM_OBJS=sdlmain.o common.o libdir.o z80.o debug.o fdc.o
GNC100EM_OBJS=gtkmain.o common.o libdir.o z80.o debug.o fdc.o
XNC100EM_OBJS=xmain.o common.o libdir.o z80.o debug.o fdc.o

# All targets build the tty version, as that should work on all
# machines any of the other versions work on. Ditto for zcntools.

all: x zcntools

x: gtk xlib zcntools

xlib: xnc100em zcntools

gtk: gnc100em zcntools

sdl: dnc100em zcntools

xnc100em: $(XNC100EM_OBJS)
	$(CC) -o xnc100em $(XNC100EM_OBJS) -L$(XROOT)/lib -lXext -lX11

gnc100em: CFLAGS += $(shell pkg-config --cflags gtk+-2.0)
gnc100em: LIBS = $(shell pkg-config --libs gtk+-2.0)
gnc100em: $(GNC100EM_OBJS)
	$(CC) -o gnc100em $(GNC100EM_OBJS) $(LIBS)

dnc100em: CFLAGS += $(shell pkg-config --cflags sdl2)
dnc100em: LIBS = $(shell pkg-config --libs sdl2) -lm
dnc100em: $(DNC100EM_OBJS)
#	$(CC) -o dnc100em $(DNC100EM_OBJS)  `sdl-config --libs`
#	$(CC) -framework SDL -framework AppKit -framework Cocoa -o dnc100em $(DNC100EM_OBJS)
#  Compile a distrubution version (using Framework SDL on OS X) like this:
	#g++ -c -o SDMain.o  SDLMain.m
	gcc -o dnc100em sdlmain.o common.o libdir.o z80.o debug.o fdc.o $(LIBS)

# special stuff needed for gtkmain.o
gtkmain.o: gtkmain.c nc100.xpm
	$(CC) $(CFLAGS) $(LDFLAGS) -c gtkmain.c -o gtkmain.o

zcntools: zcntools.o libdir.o
	$(CC) -o zcntools zcntools.o libdir.o

pdrom.h: pdrom.bin mkpdromhdr
	./mkpdromhdr <pdrom.bin >pdrom.h

# this is omitted from the `all' target as most people won't have
# zmac, and `pdrom.bin' will be effectively portable anyway... :-)
pdrom.bin: pdrom.z
	zmac pdrom.z

installdirs:
	/bin/sh ./mkinstalldirs $(BINDIR) $(BINDIR) $(MANDIR)

install: installdirs
	install -m 644 nc100em.1 makememcard.1 $(MANDIR)

	if [ -f gnc100em ]; then \
	install -m 755 -s gnc100em $(BINDIR); \
	ln -sf $(MANDIR)/nc100em.1 $(MANDIR)/gnc100em.1; \
	fi

	if [ -f xnc100em ]; then \
	install -m 755 -s xnc100em $(BINDIR); \
	ln -sf $(MANDIR)/nc100em.1 $(MANDIR)/xnc100em.1; \
	fi

	if [ -f dnc100em ]; then \
	install -m 755 -s dnc100em $(BINDIR); \
	ln -sf $(MANDIR)/nc100em.1 $(MANDIR)/dnc100em.1; \
	fi

	install -m 755 makememcard.sh $(BINDIR)/makememcard

	if [ -f znctools ]; then \
	install -m 755 -s zcntools $(BINDIR); \
	install -m 644 nc100em.1 makememcard.1 zcntools.1 $(MANDIR); \
	
	for i in cat df format get info ls put ren rm zero; do \
	  ln -sf $(BINDIR)/zcntools $(BINDIR)/zcn$$i; \
	  ln -sf $(MANDIR)/zcntools.1 $(MANDIR)/zcn$$i.1; \
	done; \
	fi

# we also blast any `nc100em' executable, in case they had a previous version.
uninstall:
	$(RM) $(BINDIR)/[dgx]nc100em
	$(RM) $(BINDIR)/makememcard
	$(RM) $(BINDIR)/zcntools
	$(RM) $(MANDIR)/{,g,s,t,x}nc100em.1* $(MANDIR)/zcntools.1*
	for i in cat df format get info ls put ren rm zero; do \
	  $(RM) $(BINDIR)/zcn$$i $(MANDIR)/zcn$$i.1*; \
	done

clean:
	$(RM) *.o *~ *.lst [dgstx]nc100em zcntools
	$(RM) mkpdromhdr pdrom.h


common.o: common.c pdrom.h z80.h libdir.h

VERS=1.2.1

tgz: ../nc100em-$(VERS).tar.gz
  
../nc100em-$(VERS).tar.gz: clean
	$(RM) ../nc100em-$(VERS)
	@cd ..;ln -s nc100em nc100em-$(VERS)
	cd ..;tar zchvf nc100em-$(VERS).tar.gz nc100em-$(VERS)
	@cd ..;$(RM) nc100em-$(VERS)
