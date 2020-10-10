RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${RED}Testing Linux...${NC}"
sudo ../../packetdrill --local_ip=192.168.0.2 --remote_ip=192.0.2.1 --tolerance_usecs=200000000 \
 --tun_id=1 $1

echo -e "${RED}Testing with so file...${NC}"
sudo ../../packetdrill --local_ip=192.168.0.2 --remote_ip=192.0.2.1 --so_filename=1 --tolerance_usecs=200000000 \
 --tun_id=1 $1