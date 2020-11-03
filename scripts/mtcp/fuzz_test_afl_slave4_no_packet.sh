/bin/AFLCplusplus -i input -o output -f fuzz_test_input4 -S fuzzer4 \
../../fuzz_test --local_ip=192.168.1.4 --remote_ip=192.0.1.4 --so_filename=1 --tolerance_usecs=200000000 \
 --fuzz_file_name=fuzz_test_input4 \
 --no_fuzz_packet \
 --tun_id=4