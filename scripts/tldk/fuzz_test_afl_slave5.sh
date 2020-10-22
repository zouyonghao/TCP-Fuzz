/bin/AFLCplusplus -i input -o output -f fuzz_test_input5 -S fuzzer5 -b \
../../fuzz_test --local_ip=192.168.1.5 --remote_ip=192.0.1.5 --so_filename=../../libtldk.so --tolerance_usecs=200000000 \
 --fuzz_file_name=fuzz_test_input5 \
 --tun_id=5