# mode="construct_from_autopan"
# # path="/home/wuliwei/dataset/bbnc/giga_image/"
# path="/media/wlw/data/NB_BBNC/giga_video/zijing16/"
# ./build/construct/construct $mode $path

# mode="construct_video"
# raw_video="/media/wlw/data/NB_BBNC/2015-06-20-zijing16/video_selected/MVI_6878.MOV"
# path_out="/media/wlw/data/NB_BBNC/giga_video/zijing16/video/6878/"
# ./build/construct/construct $mode $raw_video $path_out

mode="cut_video"
name="MVI_6881"
name_src="/media/wlw/data/NB_BBNC/2015-06-20-zijing16/video_selected/${name}.MOV"
name_dst="/media/wlw/data/NB_BBNC/2015-06-20-zijing16/video_selected_cut/${name}.avi"
./build/construct/construct $mode $name_src $name_dst
