# mode="construct"
# mode_internal="giga_image_meta"
# path="/home/bbnc/data/main_building/"
# meta=${path}"meta.conf"
# ./build/giga_video $mode $mode_internal $path $meta

# mode="construct"
# mode_internal="cut_video"
# name="MVI_7308"
# path_src="/home/wuliwei/ramdisk/video_raw/${name}.MOV"
# path_dst="/home/wuliwei/ramdisk/video_dst/${name}.avi"
# ./build/giga_video $mode $mode_internal $path_src $path_dst


mode="construct"
mode_internal="construct_camera_set_video_manual"
path_scene="/home/bbnc/data/main_building/"
# mode_video="video"
# name_video="MVI_7302"
mode_video="fly2"
name_video="0"
./build/giga_video $mode $mode_internal $path_scene $mode_video $name_video


# mode="construct"
# mode_internal="video_pyramid"
# path="/home/wuliwei/ramdisk/zijing16/video/"
# ./build/giga_video $mode $mode_internal $path