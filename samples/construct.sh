# mode="construct"
# mode_internal="giga_image_meta"
# path="$HOME/wuliwei/data/main_building/"
# meta=${path}"info_scene.txt"
# ./build/giga_video $mode $mode_internal $path $meta

mode="construct"
mode_internal="construct_camera_set_video_manual"
path_scene="$HOME/wuliwei/data/main_building/"
mode_video="fly2" # "video" or "fly2"
name_video="4" # "0" or "MVI_7302" or ...
./build/giga_video $mode $mode_internal $path_scene $mode_video $name_video

mode="construct"
mode_internal="video_pyramid"
path_scene="$HOME/wuliwei/data/main_building/video/"
mode_video="fly2"
# name_video="1"
./build/giga_video $mode $mode_internal $path_scene $mode_video $name_video

######################################################################
# mode="construct"
# mode_internal="cut_video"
# name="MVI_7308"
# path_src="/home/wuliwei/ramdisk/video_raw/${name}.MOV"
# path_dst="/home/wuliwei/ramdisk/video_dst/${name}.avi"
# ./build/giga_video $mode $mode_internal $path_src $path_dst