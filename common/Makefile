#
#  Copyright 2022 Hewlett Packard Enterprise Development LP
#
#     ALL RIGHTS RESERVED
#

CURRENT=.
TARGET=$(CURRENT)/libftiutil.a

SRCS := $(wildcard $(CURRENT)/*.cpp)
OBJS := $(SRCS:.cpp=.o)
INC := $(CURRENT)

utils.o: utils.cpp 
	@echo "PWD is $(CURRENT)"
	$(CXX) $(CXXFLAGS) -c $<  -o $@
	$(AR) $(ARFLAGS) $(TARGET) $@
	$(AR) -ts $(TARGET)

$(TARGET): utils.o
	@echo "Calling AR"
	$(AR) $(ARFLAGS) $(TARGET) $@
	$(AR) -ts $(TARGET)

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
