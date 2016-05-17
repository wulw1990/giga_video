mode="demo"

server_ip="127.0.0.1"
port=5002

echo "ip: " $server_ip
echo "port: " $port

./build/player_client/player_client $mode $server_ip $port
