# mode="demo"
# mode_internal="giga_image"
# path="${HOME}/wuliwei/data/main_building/"
# ./build/giga_video $mode $mode_internal $path

mode="demo"
mode_internal="giga_video"
path="${HOME}/wuliwei/data/main_building/"
mode_video="2"
mode_interact="mannual"
./build/giga_video $mode $mode_internal $path $mode_video $mode_interact

# mode="demo"
# mode_internal="camera_set"
# ./build/giga_video $mode $mode_internal


# mode="demo"
# mode_internal="pyramid_camera"
# ./build/giga_video $mode $mode_internal


# mode="demo"
# mode_internal="py_camera_set"
# path="/home/bbnc/data/main_building/"
# mode_video="fly2"
# name_video="0"
# ./build/giga_video $mode $mode_internal $path $mode_video $name_video
