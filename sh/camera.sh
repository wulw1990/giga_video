# mode="demo"
# scale=8
# ./build/camera/camera $mode $scale

mode="record"
path="/home/bbnc/rec/"
n_frames=10
./build/camera/camera $mode $path $n_frames
