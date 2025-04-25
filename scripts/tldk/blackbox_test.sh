
# while true; do

while true; do
    dd if=/dev/urandom of=./fuzz_test_input1 bs=1M count=1
    timeout 5 ../../fuzz_test --local_ip=192.168.0.3 --remote_ip=192.0.0.3 --so_filename=1 --tolerance_usecs=2000000000 --fuzz_file_name=fuzz_test_input1 --tun_id=1 --no_abort
    sleep 1
done
