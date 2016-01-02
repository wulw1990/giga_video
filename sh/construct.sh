# mode="construct_from_autopan"
# # path="/home/wuliwei/dataset/bbnc/giga_image/"
# path="/home/bbnc/data/20160101-pano/main_buildingdata/_group_0___mg_9234___36/"
# ./build/construct/construct $mode $path


# name="MVI_7294"

# mode="cut_video"
# name_src="/media/wuliwei/data/NB_BBNC/2015-06-20-zijing16/video_selected/${name}.MOV"
# name_dst="/media/wuliwei/data/NB_BBNC/2015-06-20-zijing16/video_selected_cut/${name}.avi"
# ./build/construct/construct $mode $name_src $name_dst

# mode="construct_video"
# path_img="/media/wuliwei/data/NB_BBNC/giga_video/zijing16/"
# raw_video_name="/media/wuliwei/data/NB_BBNC/2015-06-20-zijing16/video_selected_cut/${name}.avi"
# path_output="/media/wuliwei/data/NB_BBNC/giga_video/zijing16/video/${name}/"
# ./build/construct/construct $mode $path_img $raw_video_name $path_output
# ls /media/wuliwei/data/NB_BBNC/giga_video/zijing16/video/ | grep -e "^M" > /media/wuliwei/data/NB_BBNC/giga_video/zijing16/video/list.txt

# mode="construct_video"
# name="MVI_6878"
# path_img="/home/wuliwei/dataset/bbnc/zijing16/"
# raw_video_name="/home/wuliwei/dataset/bbnc/${name}.avi"
# path_output="/home/wuliwei/dataset/bbnc/zijing16/video/${name}/"
# ./build/construct/construct $mode $path_img $raw_video_name $path_output



# mode="test_geo_align"
# scene="../win_match.jpg"
# frame="../frame.jpg"
# ./build/construct/construct $mode $scene $frame



mode="construct_camera_set"
# path="/media/wuliwei/data/NB_BBNC/giga_video/zijing16-slave1/"
path="/home/bbnc/data/20160101-pano/main_buildingdata/_group_0___mg_9234___36/"
./build/construct/construct $mode $path
