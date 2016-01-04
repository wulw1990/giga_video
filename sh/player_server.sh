mode="demo"

# path="/media/wuliwei/data/NB_BBNC/giga_video/zijing16-master/"
#path="/home/bbnc/data/20160101-pano/main_buildingdata/_group_0___mg_9234___36/"
path="/media/wuliwei/data/NB_BBNC/giga_video/zijing16-master/"
port=5001

LC_ALL=C ifconfig  | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'

echo "port: " $port

./build/player_server/player_server $mode $path $port
