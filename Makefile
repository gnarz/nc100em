# makefile for nc100em

# version
VERS=1.2.1

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
# -DDEBUG enables extra debugging output from various parts of the emulator.
#  You won't need this for normal operation.
#
# As I say, the default settings below should generally be ok.
#
CFLAGS=-DMITSHM -DUSE_MMAP_FOR_CARD -DSCALE=1 -Os -Wall -I$(XROOT)/include
#CFLAGS=-DMITSHM -DUSE_MMAP_FOR_CARD -DSCALE=1 -g -DDEBUG -Wall -I$(XROOT)/include

# dest for make install
#
PREFIX=/usr/local
BINDIR=$(PREFIX)/bin
MANDIR=$(PREFIX)/man/man1

# you shouldn't need to edit the rest
#-----------------------------------------------------------------

# this looks wrong, but *ops.c are actually #include'd by z80.c
COMMON_OBJS=common.o libdir.o z80.o debug.o fdc.o
DNC100EM_OBJS=sdlmain.o $(COMMON_OBJS)
GNC100EM_OBJS=gtkmain.o $(COMMON_OBJS)
XNC100EM_OBJS=xmain.o $(COMMON_OBJS)

# All targets build the tty version, as that should work on all
# machines any of the other versions work on. Ditto for zcntools.

all:;
	@echo "choose one or more of these targets:"
	@echo "  x         builds xlib and gtk versions"
	@echo "  xlib      builds xlib version"
	@echo "  gtk       builds gtk version"
	@echo "  sdl       builds sdl version"
	@echo "  tools     builds ncconvert"
	@echo "  zcn       builds zcntools"
	@echo "  install   installs all built tools below \$$(PREFIX)"
	@echo "  uninstall removes all installed files below \$$(PREFIX)"
	@echo "  clean     cleans source directory"
	@echo "  tgz       builds compressed tarball of source directory"

x: gtk xlib

xlib: xnc100em

gtk: gnc100em

sdl: dnc100em

tools: ncconvert

zcn: zcntools

xnc100em: LIBS=-L$(XROOT)/lib -lXext -lX11
xnc100em: $(XNC100EM_OBJS)
	$(CC) -o $@ $^ $(LIBS)

gnc100em: CFLAGS += $(shell pkg-config --cflags gtk+-2.0)
gnc100em: LIBS = $(shell pkg-config --libs gtk+-2.0)
gnc100em: $(GNC100EM_OBJS)
	$(CC) -o $@ $^ $(LIBS)

dnc100em: CFLAGS += $(shell pkg-config --cflags sdl2)
dnc100em: LIBS = $(shell pkg-config --libs sdl2) -lm
dnc100em: $(DNC100EM_OBJS)
	$(CC) -o $@ $^ $(LIBS)

# special stuff needed for gtkmain.o
gtkmain.o: gtkmain.c nc100.xpm
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c $<

common.o: common.c pdrom.h z80.h libdir.h

ncconvert: ncconvert.o mini_utf8.h
	$(CC) -o $@ $<

zcntools: zcntools.o libdir.o
	$(CC) -o $@ $^

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
	install -m 644 zcntools.1 $(MANDIR); \
	fi
	
	if [ -f ncconvert ]; then \
	install -m 755 -s ncconvert $(BINDIR); \
	#install -m 644 ncconvert.1 $(MANDIR); \
	fi

	for i in cat df format get info ls put ren rm zero; do \
	  ln -sf $(BINDIR)/zcntools $(BINDIR)/zcn$$i; \
	  ln -sf $(MANDIR)/zcntools.1 $(MANDIR)/zcn$$i.1; \
	done; \
	fi

# we also blast any `nc100em' executable, in case they had a previous version.
uninstall:
	$(RM) $(BINDIR)/[dgx]nc100em
	$(RM) $(BINDIR)/ncconvert
	$(RM) $(BINDIR)/makememcard
	$(RM) $(BINDIR)/zcntools
	$(RM) $(MANDIR)/*nc100em.1
	$(RM) $(MANDIR)/zcntools.1 $(MANDIR)/makememcard.1
	for i in cat df format get info ls put ren rm zero; do \
	  $(RM) $(BINDIR)/zcn$$i $(MANDIR)/zcn$$i.1*; \
	done

clean:
	$(RM) *.o *~ *.lst [dgstx]nc100em zcntools ncconvert
	$(RM) mkpdromhdr pdrom.h


tgz: ../nc100em-$(VERS).tar.gz
  
../nc100em-$(VERS).tar.gz: clean
	$(RM) ../nc100em-$(VERS)
	@cd ..;ln -s nc100em nc100em-$(VERS)
	cd ..;tar zchvf nc100em-$(VERS).tar.gz nc100em-$(VERS)
	@cd ..;$(RM) nc100em-$(VERS)
