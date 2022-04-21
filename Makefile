#
#  Copyright 2022 Hewlett Packard Enterprise Development LP
#
#     ALL RIGHTS RESERVED
#

# Top level Makefile.
# Uses GNU Makefile syntax. 
# Use /usr/coreutils/bin/make when building natively on NonStop.

export TOPDIR = $(PWD)

ifdef COMP_ROOT
export CXX = $(COMP_ROOT)/usr/bin/c11
export CC = $(COMP_ROOT)/usr/bin/c11
export AR = $(COMP_ROOT)/usr/bin/ar
else
export CXX = /usr/bin/c11
export CC = /usr/bin/c11
export AR = /usr/bin/ar
endif

OPT = -O2
COMMON_DIR = $(TOPDIR)/common

export CFLAGS = $(OPT) -g -Wextensions -D_XOPEN_SOURCE_EXTENDED 
export CPPFLAGS = -D_XOPEN_SOURCE_EXTENDED 

export CXXFLAGS = $(OPT) -Wlp64 -g -Wextensions -D_XOPEN_SOURCE_EXTENDED \
	   -I$(COMMON_DIR)
export CXXLIBS = -lcppc -lcpp4
export FTILIB = -lnsfti
export LIBUTIL = -L$(COMMON_DIR) -lftiutil

TOPTARGETS := all clean

SUBDIRS := $(wildcard */.)

$(TOPTARGETS): $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
