sudo ASAN_SYMBOLIZER_PATH=/usr/lib/llvm-6.0/bin/llvm-symbolizer ASAN_OPTIONS=detect_leaks=0:allocator_may_return_null=1 \
    ../../fuzz_test \
    --local_ip=192.168.0.1 --remote_ip=192.0.0.2 \
    --so_filename=1 --tolerance_usecs=100000000 \
    --tun_id=6 \
    --fuzz_file_name=$1
