/bin/AFLCplusplus -i input -o output -f fuzz_test_input1 -M fuzzer1 -b \
 ../../fuzz_test --local_ip=192.168.0.2 --remote_ip=192.0.2.1 --so_filename=1 --tolerance_usecs=200000000 \
 --fuzz_file_name=fuzz_test_input1 \
 --tun_id=1