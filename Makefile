TOP_DIR	= $(shell pwd)
BASE_DIR 		= $(TOP_DIR)/tnl/base
NET_DIR  		= $(TOP_DIR)/tnl/net
TEST_DIR 		= $(TOP_DIR)/test
EXAMPLES_DIR	= $(TOP_DIR)/examples

export TOP_DIR BASE_DIR NET_DIR TEST_DIR EXAMPLES_DIR

CXX			= g++
LD			= ld
CXXFLAGS	= -g -I $(TOP_DIR) -std=c++11 -pthread
LDFLAGS		= 

export CXX LD CXXFLAGS LDFLAGS

.PHONY : all base_build net_build test_build
all : base_build net_build test_build

base_build:
	make -C $(BASE_DIR)

net_build :
	make -C $(NET_DIR)

test_build :
	make -C $(TEST_DIR)

examples_build :
	make -C $(EXAMPLES_DIR)

.PHONY : clean base_clean net_clean test_clean
clean : base_clean net_clean test_clean

base_clean :
	make -C $(BASE_DIR) clean

net_clean :
	make -C $(NET_DIR) clean

test_clean :
	make -C $(TEST_DIR) clean

examples_clean:
	mkae -C $(EXAMPLES_DIR) clean