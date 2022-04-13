#
#  Copyright 2021 Hewlett Packard Enterprise Development LP
#
#     ALL RIGHTS RESERVED
#

CURRENT=.
TARGET=$(CURRENT)/bin/TEST_APP

LIB_FOLDER= ../../../lib/
LIBS=

OPT=O2
DATA_MODEL=Wlp64

ifdef COMP_ROOT
	export CC = "$(COMP_ROOT)/usr/bin/c11"
else
	export CC = c11
endif

SRCS := $(wildcard $(CURRENT)/src/*.cpp)
OBJS := $(SRCS:.cpp=.o)
INC := $(CURRENT)/include/

INC_LIB= ../../../include/

	
%.o: %.cpp 
	$(CC) -g  -$(OPT)  -Wextensions   -D_XOPEN_SOURCE_EXTENDED -I$(INC) -I$(INC_LIB) -$(DATA_MODEL)   -c -o $@ $<	

$(TARGET):	 $(OBJS)
	$(CC)  -o $(TARGET) $(OBJS)  $(LIBS) -L $(LIB_FOLDER) -lnsfti -$(DATA_MODEL)  -Wsaveabend  -lcppc -lcpp4


all:	$(TARGET) 


clean:
	rm -f $(OBJS) $(TARGET)
