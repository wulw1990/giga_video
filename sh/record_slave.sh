mode="record_slave"
path="../record_slave/"
n_frames=10
ip="127.0.0.1"
port=5000
./build/camera/camera $mode $path $n_frames $ip $port
