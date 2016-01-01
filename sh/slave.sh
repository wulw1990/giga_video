mode="demo"

server_ip="127.0.0.1"
port=5002

echo "ip: " $server_ip
echo "port: " $port

./build/slave/slave $mode $server_ip $port
