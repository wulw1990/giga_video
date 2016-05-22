# mode="construct"
# mode_internal="giga_image_meta"
# path="/mnt/hgfs/fast/zijing16/"
# meta="/mnt/hgfs/fast/zijing16/meta.conf"
# ./build/giga_video $mode $mode_internal $path $meta


# mode="construct"
# mode_internal="video_pyramid"
# path="/home/wuliwei/ramdisk/zijing16/video/"
# ./build/giga_video $mode $mode_internal $path


# mode="construct"
# mode_internal="cut_video"
# name="MVI_7308"
# path_src="/home/wuliwei/ramdisk/video_raw/${name}.MOV"
# path_dst="/home/wuliwei/ramdisk/video_dst/${name}.avi"
# ./build/giga_video $mode $mode_internal $path_src $path_dst


mode="construct"
mode_internal="construct_camera_set"
path="/home/wuliwei/ramdisk/zijing16/"
./build/giga_video $mode $mode_internal $path