#
#  Copyright 2021 Hewlett Packard Enterprise Development LP
#
#     ALL RIGHTS RESERVED
#

CURRENT=.
TARGET=$(CURRENT)/bin/OLTP-App

SRCS := $(wildcard $(CURRENT)/src/*.cpp)
OBJS := $(SRCS:.cpp=.o)
INC = -I$(CURRENT)/include

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

$(TARGET): $(OBJS)	
	$(CXX)  -o $(TARGET) $(CXXFLAGS) $(OBJS) $(LIBUTIL) $(FTILIB) \
                -Wsaveabend $(CXXLIBS)

all: $(TARGET) 

clean:
	rm -f $(OBJS) $(TARGET)
