#
#  Copyright 2021 Hewlett Packard Enterprise Development LP
#
#     ALL RIGHTS RESERVED
#

CURRENT=.
TARGET=$(CURRENT)/bin/HA-App

SRCS := $(wildcard $(CURRENT)/src/*.cpp)
OBJS := $(SRCS:.cpp=.o)
INC := $(CURRENT)/include/


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(CXXFLAGS) $(FTILIB) \
		-Wsaveabend $(CXXLIBS) $(LIBUTIL)

all:    $(TARGET) 

clean:
	rm -f $(OBJS) $(TARGET)
