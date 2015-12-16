mode="demo"

path="/media/wuliwei/data/NB_BBNC/giga_video/zijing16/"
port=5002

LC_ALL=C ifconfig  | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'

echo "port: " $port

./build/player_server/player_server $mode $path $port
