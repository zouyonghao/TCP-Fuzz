MTCP_ROOT=/home/zyh/Desktop/mtcp/mtcp/lib
# DPDK_BUILD=/home/zyh/Desktop/mtcp/dpdk/x86_64-native-linuxapp-gcc
# DPDK_BUILD=/home/zyh/Desktop/tldk-2/dpdk/_build/dpdk/x86_64-native-linuxapp-gcc
# DPDK_MACHINE_LINKER_FLAGS=${DPDK_BUILD}/lib/ldflags.txt
# LIBS=$(shell cat ${DPDK_MACHINE_LINKER_FLAGS})
# DPDK_MACHINE_LDFLAGS=-ldpdk

DPDK_LIBS_PATH := /home/zyh/Desktop/tldk-2/dpdk/install/lib
$(eval LIBS = $(wildcard $(DPDK_LIBS_PATH)/librte*.a))
packetdrill-ext-libs := -static -L$(MTCP_ROOT) -Wl,--whole-archive \
						${LIBS} -lmtcp -Wl,--no-whole-archive -lnuma -lpthread -ldl -lrt -lpcap -lgmp 
include Makefile.common
