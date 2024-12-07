USAGECPP_DIR?=./usage
INCLUDES += -I$(USAGECPP_DIR)/include

USAGE_OBJS:= \
	objs/usage.o \
	objs/usage_validator.o

objs/usage_validator.o: $(USAGECPP_DIR)/src/validator.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/usage.o: $(USAGECPP_DIR)/src/usage.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;
