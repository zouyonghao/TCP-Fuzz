/bin/AFLCplusplus -i input -o output -f fuzz_test_input3 -S fuzzer3 -b \
 ../../fuzz_test --local_ip=192.168.1.3 --remote_ip=192.0.1.3 --so_filename=../../libtldk.so --tolerance_usecs=200000000 \
 --fuzz_file_name=fuzz_test_input3 \
 --tun_id=3 \
 --no_fuzz_receive_packets_in_background