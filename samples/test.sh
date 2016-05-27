# mode="test"
# mode_internal="giga_image_meta"
# path="/mnt/hgfs/fast/zijing16/"
# meta="/mnt/hgfs/fast/zijing16/meta.conf"
# ./build/giga_video $mode $mode_internal $path $meta


# mode="test"
# mode_internal="read_image_tile"
# path="/home/wuliwei/ramdisk/zijing16/0/"
# ./build/giga_video $mode $mode_internal $path


# mode="test"
# mode_internal="read_web_video"
# video_name="http://127.0.0.1:8090/test.rm"
# ./build/giga_video $mode $mode_internal $video_name

# mode="test"
# mode_internal="write_web_video"
# ./build/giga_video $mode $mode_internal


# mode="test"
# mode_internal="virtual_camera_device"
# device="/dev/video8"
# ./build/giga_video $mode $mode_internal $device


# mode="test"
# mode_internal="video_writer_v4l2"
# device="/dev/video8"
# ./build/giga_video $mode $mode_internal $device


# mode="test"
# mode_internal="frame_to_ffm"
# input_video="../MVI_7305.avi"
# path_tmp="../tmp/"
# ffm="http://localhost:8090/feed1.ffm"
# ./build/giga_video $mode $mode_internal $input_video $path_tmp $ffm



mode="test"
mode_internal="write_jpge_pipe"
./build/giga_video $mode $mode_internal


# mode="test"
# mode_internal="zncc"
# name_frame="./data/align/frame.jpg"
# name_refer="./data/align/refer.jpg"
# scale=8
# ./build/giga_video $mode $mode_internal $name_frame $name_refer $scale