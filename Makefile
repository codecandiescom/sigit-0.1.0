# $Id: Makefile,v 0.7 2001/08/13 16:43:00 redhead Exp $

# Makefile for sigit

# OS
OS=$(shell uname -s)

# Compiler
CC=gcc

# Install prefix Where the database and original file is placed.. 

PREFIX=/usr/share/sigit

# Installation directory -- where the binary will go

INSTALLDIR=/usr/bin

# Where the default sigit configuration will be

RCDIR=/etc

# Where the systems man pages are placed.. 

MANDIR=/usr/share/man

# if you want to test something..

FLAGS=

# Link libraries
#LIBS=	`gtk-config --libs`

# lets make our desired architecture.. 
ifeq ($(OS), Linux)
OS=	LINUX
endif

# havn't tried it on FreeBSD yet..
#ifeq ($(OS), FreeBSD)
#OS=	FREEBSD
#endif


ifeq ($(OS), HP-UX)
OS=	HPUX
endif

# Compiler flags
CFLAGS= -Wall -O2 -D$(OS) $(FLAGS)

# This is needed for "make install"
OWNER  = root
GROUP  = root
INSTALL  = /usr/bin/install -o $(OWNER) -g $(GROUP) 


# ----------- You shouldn't need to make changes below here. -------------

VERSION= 0.1.0

OBJS=	config.o deamoninit.o files.o sigit.o

EXE=	sigit

all: sigit sigit.1 sigit.rc.1

sigit:	$(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS)
	@echo  
	@echo Finished making $(EXE) for the $(OS) architecture. 
	@echo  

sigit.o:	sigit.c sigit.h config.h
	$(CC) $(CFLAGS) -c sigit.c -o sigit.o

deamoninit.o:	deamoninit.c deamoninit.h config.h
	$(CC) $(CFLAGS) -c deamoninit.c -o deamoninit.o

config.o:	config.c config.h
	$(CC) $(CFLAGS) -c config.c -o config.o

files.o:	files.c files.h config.h
	$(CC) $(CFLAGS) -c files.c -o files.o

$(OBJS):	sigit.h deamoninit.h config.h files.h

sigit.1:	sigit.pod
	pod2man --section=1 --release="Amusements" \
	--center="By Kenneth 'Redhead' Nielsen" \
	--official sigit.pod > sigit.1


sigit.rc.1:	sigit.rc.pod
	pod2man --section=1 --release="Amusements" \
	--center="By Kenneth 'Redhead' Nielsen" \
	--official sigit.rc.pod > sigit.rc.1


install:
	$(INSTALL) -d $(PREFIX)
	$(INSTALL) -d $(INSTALLDIR)
	$(INSTALL) -d $(RCDIR)
	$(INSTALL) -d $(MANDIR)/man1
	$(INSTALL) -m 555 $(EXE) $(INSTALLDIR)
	$(INSTALL) -m 664 sigit.data $(PREFIX)
	$(INSTALL) -m 664 sigit.sig $(PREFIX)
	$(INSTALL) -m 664 sigit.rc $(RCDIR)
	$(INSTALL) -m 444 sigit.1 $(MANDIR)/man1
	$(INSTALL) -m 444 sigit.rc.1 $(MANDIR)/man1


# Tidy up after ourselves
clean:
	-rm -f  *.o *~

distclean: clean
	-rm -f $(EXE) core *.1 \#*\# *.save


uninstall:
	rm -rf $(PREFIX)
	rm -f $(INSTALLDIR)/$(EXE)
	rm -f $(RCDIR)/sigit.rc
	rm -f $(MANDIR)/man1/sigit.*












