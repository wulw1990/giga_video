mode="record_master"
path="../record_master/"
n_frames=10
n_slaves=2
port=5000
./build/camera/camera $mode $path $n_frames $n_slaves $port
