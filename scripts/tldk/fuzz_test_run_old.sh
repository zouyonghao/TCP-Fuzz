sudo ASAN_OPTIONS=detect_leaks=0:allocator_may_return_null=1 \
   ../../fuzz_test\
    --local_ip=192.168.1.0 --remote_ip=192.0.0.2 \
     --so_filename=/home/$(USER)/Desktop/tldk-old/libtldk.so --tolerance_usecs=10000000 \
    --tun_id=0 \
    ./fuzz_test.pkt