mode="demo"

path="/media/wuliwei/data/NB_BBNC/giga_video/zijing16-slave1/"

server_ip="127.0.0.1"
port=5001

echo "ip: " $server_ip
echo "port: " $port

name="slave1"

./build/slave/slave $mode $path $server_ip $port $name
