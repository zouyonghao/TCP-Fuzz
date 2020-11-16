/bin/AFLCplusplus -i input -o output -f fuzz_test_input5 -S fuzzer5 \
../../fuzz_test --local_ip=192.168.1.5 --remote_ip=192.0.1.5 --so_filename=1 --tolerance_usecs=200000000 \
 --fuzz_file_name=fuzz_test_input5 \
 --fuzz_without_dependency \
 --tun_id=5 \
 --no_fuzz_receive_packets_in_background