#
#  Copyright 2022 Hewlett Packard Enterprise Development LP
#
#     ALL RIGHTS RESERVED
#

# Top level Makefile.
# Uses GNU Makefile syntax. Use /usr/coreutils/bin/make

export TOPDIR = $(CURDIR)
export 



CXXFLAGS = -O2 -Wlp64 -g -Wextensions -D_XOPEN_SOURCE_EXTENDED -I$(INCLUDE_DIR)

TOPTARGETS := all clean

SUBDIRS := $(wildcard */.)

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
