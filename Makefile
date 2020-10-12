TLDK_ROOT=/home/zyh/Desktop/tldk-2
packetdrill-ext-libs := -static -L$(TLDK_ROOT) -Wl,--whole-archive -ltldk -Wl,--no-whole-archive -lnuma -lpthread -lrt -ldl -lpcap
CFLAGS += -D_TLDK
include Makefile.common
