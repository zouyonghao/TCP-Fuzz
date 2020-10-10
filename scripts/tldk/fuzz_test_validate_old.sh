rm log

walk_dir () {
    for pathname in "$1"/*; do
        cp "$pathname" ./fuzz_test_input
        if [[ $? != 0 ]]; then
            continue
        fi
        echo "$pathname"
        if../../fuzz_test--local_ip=192.168.0.2 --remote_ip=192.0.0.2  --so_filename=/home/zyh/Desktop/tldk-old/libtldk.so \
        --tolerance_usecs=10000000 --fuzz_file_name=fuzz_test_input ./fuzz_test.pkt 2>log | grep -q "TLDK differs"; then
            echo "Find different!"
            if ./fuzz_test_remove_duplicate unique_error_seqs log; then
                echo "Find unique"
                cp "$pathname" ./
            fi
        fi
    done
}

OUTPUT_DIR=./output

walk_dir "$OUTPUT_DIR"/crash


walk_dir "$OUTPUT_DIR"/fuzzer1/crash
walk_dir "$OUTPUT_DIR"/fuzzer2/crash
walk_dir "$OUTPUT_DIR"/fuzzer3/crash
walk_dir "$OUTPUT_DIR"/fuzzer4/crash
walk_dir "$OUTPUT_DIR"/fuzzer5/crash