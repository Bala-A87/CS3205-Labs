#
# Internetworking with TCP/IP, Volume III example code Makefile
#
#	David L Stevens, Internetworking Research Group at Purdue
#	Tue Sep 17 19:40:42 EST 1991
#

INCLUDE =

CLNTS = UDPecho UDPmath TCPfile
SERVS = UDPechod UDPmathd TCPfiled
OTHER = superd TCPmtechod

DEFS = 
CFLAGS = -W -pedantic -ansi -g -lm ${DEFS} ${INCLUDE}

HDR =
CSRC = UDPecho.c UDPmath.c TCPfile.c
CXSRC = connectsock.c errexit.c

SSRC = UDPechod.c UDPmathd.c TCPfiled.c
SXSRC = passivesock.c sv_funcs.c

CXOBJ = connectsock.o errexit.o
SXOBJ = passivesock.o errexit.o

PROGS = ${CLNTS} ${SERVS} ${OTHER}

all: ${PROGS}

${CLNTS}: ${CXOBJ}
	${CC} -o $@ ${CFLAGS} $@.o ${CXOBJ}

${SERVS}: ${SXOBJ}
	${CC} -o $@ ${CFLAGS} $@.o ${SXOBJ}

superd: ${SXOBJ} superd.o sv_funcs.o
	${CC} -o $@ ${CFLAGS} $@.o ${SXOBJ} sv_funcs.o

TCPmtechod: TCPmtechod.o ${SXOBJ}
	${CC} -o $@ ${CFLAGS} $@.o ${SXOBJ} -lpthread

clients: ${CLNTS}
servers: ${SERVS}

clean: FRC
	rm -f Makefile.bak a.out core errs lint.errs ${PROGS} *.o

depend: ${HDR} ${CSRC} ${SSRC} ${TNSRC} FRC
	maketd -a ${DEFS} ${INCLUDE} ${CSRC} ${SSRC} ${TNSRC}

install: all FRC
	@echo "Your installation instructions here."

lint: ${HDR} ${XSRC} ${CSRC} ${SSRC} FRC
	lint ${DEFS} ${INCLUDE} ${CSRC} ${SSRC} ${CXSRC} ${SXSRC}

print: Makefile ${SRC} FRC
	lpr Makefile ${CSRC} ${SSRC} ${CXSRC} ${SXSRC}

spotless: clean FRC
	rcsclean Makefile ${HDR} ${SRC}

tags: ${CSRC} ${SSRC} ${CXSRC} ${SXSRC}
	ctags ${CSRC} ${SSRC} ${CXSRC} ${SXSRC}

${HDR} ${CSRC} ${CXSRC} ${SSRC} ${SXSRC}:
	co $@

UDPechod: UDPechod.o
UDPecho: UDPecho.o
UDPmathd: UDPmathd.o
UDPmath: UDPmath.o
TCPfiled: TCPfiled.o
TCPfile: TCPfile.o

FRC:
	
# DO NOT DELETE THIS LINE - maketd DEPENDS ON IT
