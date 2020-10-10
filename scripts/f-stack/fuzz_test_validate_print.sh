RED='\033[0;31m'
NC='\033[0m' # No Color
walk_dir () {
    for pathname in "$1"/*; do
        cp "$pathname" ./fuzz_test_input
        echo -e "${RED}$pathname${NC}"
        sudo ../../fuzz_test --local_ip=192.168.0.2 --remote_ip=192.0.0.2 --so_filename=1 \
        --tolerance_usecs=10000000 --fuzz_file_name=fuzz_test_input --no_fuzz_receive_packets_in_background
	sleep 1
    done
}


walk_dir $1

