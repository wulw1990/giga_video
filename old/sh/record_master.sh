mode="record_master"
path="../record_master/"
n_frames=50
n_slaves=0
port=5002
setting_file="./sh/record_master_setting.xml"
./build/camera/camera $mode $path $n_frames $n_slaves $port $setting_file
