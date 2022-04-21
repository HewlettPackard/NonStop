#
#  Copyright 2021 Hewlett Packard Enterprise Development LP
#
#     ALL RIGHTS RESERVED
#

CURRENT=.
NSFTI_APP = $(CURRENT)/bin/NSFTI_App
PP_APP = $(CURRENT)/bin/PP_App
REQ = $(CURRENT)/bin/req

ifdef COMP_ROOT
	CRTLO = $(COMP_ROOT)/usr/lib/crtlnsx.o
else
	CRTLO = /usr/lib/crtlnsx.o
endif

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CPPFLAGS) -c -o $@ $<

$(NSFTI_APP): server_ftilib.cpp
	$(CXX) -o $@ $(CXXFLAGS) $< $(FTILIB) \
		-Wsaveabend $(CXXLIBS)

$(PP_APP): server_pp.c
	$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) -Wsaveabend $< $(CRTLO) 

$(REQ): requester.c
	$(CC) -o $@ $(CFLAGS) $(CPPFLAGS) -Wsaveabend $<  

all: $(NSFTI_APP) $(PP_APP) $(REQ)

clean:
	rm -f 
