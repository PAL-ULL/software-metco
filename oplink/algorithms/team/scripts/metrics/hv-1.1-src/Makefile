#-----------------------------------------------------------------------

#  Makefile for hv

VERSION := 1.1

# ---------------------------------------------------------------------

#                       Copyright (c) 2005, 2006
#                  Carlos Fonseca <cmfonsec@ualg.pt>
#             Manuel Lopez-Ibanez <m.lopez-ibanez@napier.ac.uk>
#                    Luis Paquete <lpaquete@ualg.pt>

# This program is free software (software libre); you can redistribute
# it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either 
# version 2 of the License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, you can obtain a copy of the GNU
# General Public License at:
#                  http://www.gnu.org/copyleft/gpl.html
# or by writing to:
#            Free Software Foundation, Inc., 59 Temple Place,
#                  Suite 330, Boston, MA 02111-1307 USA

#-----------------------------------------------------------------------

DEBUG=0

SRC_DIR := ./
BINDIR := ./

VARIANT ?= 4

MAIN := hv

OBJS = hv.o main-hv.o avl.o io.o timer.o

HEADERS = hv.h avl.h timer.h io.h

OBJS :=$(foreach OBJ, $(OBJS), $(join $(SRC_DIR), $(OBJ)) )
HEADERS :=$(foreach OBJ, $(HEADERS), $(join $(SRC_DIR), $(OBJ)) )


CC = gcc
march = #pentium-m # athlon-xp (see man gcc)
OPTIMISE := -O3 -funroll-loops
# -fomit-frame-pointer is enabled at levels -O, -O2, -O3, -Os.

ifneq ($(DEBUG),0)
CDEBUG = -g -DMALLOC_CHECK_=2
endif

override CFLAGS += $(OPTIMISE) -march=$(march) -Wall -W -D_GNU_SOURCE \
		   -D DEBUG=$(DEBUG) $(CDEBUG)

LINKERFLAGS =# Order is important !

DELETE = @rm -f
ECHO = @echo "$(1)"

march_error = $(error please define an architecture, e.g., 'make march=pentium')

.PHONY : all clean distclean ps test dist error

$(BINDIR)/$(MAIN)$(EXE) : $(OBJS) $(HEADERS)
ifndef march
	$(march_error)
endif
	@mkdir -p $(BINDIR)
	$(call ECHO,--> Building $(MAIN) version $(VERSION) <---)
	$(CC) -o $(BINDIR)/$(MAIN)$(EXE) $(OBJS) \
		 $(CFLAGS) \
		 $(LINKERFLAGS)

$(OBJS) : $(HEADERS)

%.o : %.c
ifndef march
	$(march_error)
endif
	$(CC) $(CFLAGS) -D VERSION=\"$(VERSION)\" -D MARCH=\"$(march)\" -D VARIANT=$(VARIANT) -c -o $@ $<

clean :
	$(call ECHO,---> Removing $(BINDIR)/$(MAIN) <---)
	$(DELETE) $(BINDIR)/$(MAIN)

	$(call ECHO,---> Removing object files <---)
	$(DELETE) $(OBJS)

all :  clean $(MAIN)

ps:
	a2ps -E -g -o hv.ps hv.c

DIST_SRC_FILES =  Makefile README LICENSE $(OBJS:.o=.c) $(HEADERS)
DIST_SRC := hv-$(VERSION)-src

dist : DEBUG=0
dist : CDEBUG=
dist :
	@(mkdir -p ../$(DIST_SRC) \
	&& rsync -rlpC --exclude=.svn $(DIST_SRC_FILES) ../$(DIST_SRC)/ \
	&& cd .. \
	&& tar cf - $(DIST_SRC) | gzip -f9 > $(DIST_SRC).tar.gz \
	&& rm -rf ./$(DIST_SRC)/* && rmdir ./$(DIST_SRC)/ \
	&& echo "$(DIST_SRC).tar.gz created." && cd $(PWD) )
