/bin/AFLCplusplus -i input -o output -f fuzz_test_input2 -S fuzzer2 \
../../fuzz_test--local_ip=192.168.1.2 --remote_ip=192.0.1.2 --so_filename=/home/$(USER)/Desktop/tldk-old/libtldk.so --tolerance_usecs=200000000 \
 --fuzz_file_name=fuzz_test_input2 \
 --no_fuzz_syscall \
 --tun_id=2