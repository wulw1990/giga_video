# mode="demo_virtual"
# video_mode="video"
# scale=2
# path="/media/wuliwei/data/NB_BBNC/giga_video/zijing16/video/"
# n_videos="4"
# video0="MVI_6878/video.avi"
# video1="MVI_6880/video.avi"
# video2="MVI_6881/video.avi"
# video3="MVI_6883/video.avi"
# ./build/camera/camera $mode $video_mode $scale $path $n_videos \
# $video0 $video1 $video2 $video3
# argc_head

mode="demo_virtual"
video_mode="image"
scale=4
path="/home/wuliwei/rec-static/"
n_videos=4
video0="0"
video1="1"
video2="2"
video3="3"
./build/camera/camera $mode $video_mode $scale $path $n_videos \
$video0 $video1 $video2 $video3
argc_head