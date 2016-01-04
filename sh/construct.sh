# mode="construct_from_autopan"
# # # path="/home/wuliwei/dataset/bbnc/giga_image/"
# # path="/media/wuliwei/data/NB_BBNC/giga_video/main_building/"
# path="/home/bbnc/data/20160104_2/panorama1data/_group_0___mg_0001___36/"
# n_layaers=7
# ./build/construct/construct $mode $path $n_layaers

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
# path="/media/wuliwei/data/NB_BBNC/giga_video/zijing16-slave1/"
# path="/media/wuliwei/data/NB_BBNC/giga_video/main_building/"
# path="/home/bbnc/data/20160101-pano/main_buildingdata/_group_0___mg_9234___36/"
path="/home/bbnc/data/20160104_2/panorama1data/_group_0___mg_0001___36/"
video_mode="fly2"
align_mode="manual"
./build/construct/construct $mode $path $video_mode $align_mode
