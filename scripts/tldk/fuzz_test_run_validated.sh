rm log

for pathname in `ls -l ./00* | awk '{print $9}'`/*; do
    cp "$pathname" ./fuzz_test_input
    echo "$pathname"
    echo "$pathname"
   ../../fuzz_test --local_ip=192.168.0.2 --remote_ip=192.0.0.2 --so_filename=../../libtldk.so --tolerance_usecs=10000000 ./fuzz_test.pkt
done
