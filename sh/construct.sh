# mode="construct_from_autopan"
# # path="/home/wuliwei/dataset/bbnc/giga_image/"
# path="/media/wuliwei/data/NB_BBNC/giga_video/main_building/"
# ./build/construct/construct $mode $path

# name="MVI_7294"
# mode="cut_video"
# name_src="/media/wuliwei/data/NB_BBNC/2015-06-20-zijing16/video_selected/${name}.MOV"
# name_dst="/media/wuliwei/data/NB_BBNC/2015-06-20-zijing16/video_selected_cut/${name}.avi"
# ./build/construct/construct $mode $name_src $name_dst

# mode="test_geo_align"
# scene="../win_match.jpg"
# frame="../frame.jpg"
# ./build/construct/construct $mode $scene $frame

mode="construct_camera_set"
path="/media/wuliwei/data/NB_BBNC/giga_video/zijing16-slave1/"
# path="/media/wuliwei/data/NB_BBNC/giga_video/main_building/"
video_mode="virtual"
align_mode="auto"
./build/construct/construct $mode $path $video_mode $align_mode
