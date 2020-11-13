/bin/AFLCplusplus -i input -o output -f fuzz_test_input1 -M fuzzer1 \
../../fuzz_test --local_ip=192.168.1.1 --remote_ip=192.0.1.1 --so_filename=1 --tolerance_usecs=200000000 \
 --fuzz_file_name=fuzz_test_input1 \
 --tun_id=1 --fuzz_once