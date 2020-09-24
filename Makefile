TLDK_ROOT=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/../../
packetdrill-ext-libs := -static -L$(TLDK_ROOT) -Wl,--whole-archive -ltldk -Wl,--no-whole-archive -lnuma -lpthread -lrt -ldl -lpcap
include Makefile.common
