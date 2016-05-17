mode="image"

# path="/home/wuliwei/dataset/bbnc/zijing16/"
# path="/media/wuliwei/data/NB_BBNC/giga_video/zijing16-slave1/"
# path="/media/wuliwei/data/NB_BBNC/giga_video/main_building/"
# path="/home/bbnc/data/20160101-pano/main_buildingdata/_group_0___mg_9234___36/"
# path="/home/bbnc/data/20160104_2/panorama1data/_group_0___mg_0001___36/"
# path="/media/wuliwei/data/NB_BBNC/0104/giga_video/"
# path="/media/wuliwei/win8/data/0104/giga_video/"
# path="/media/wuliwei/data/NB_BBNC/Xiaoyun/construct/53/"
path="/mnt/hgfs/NB_BBNC/giga_video_embed/2015-06-20-zijing16-final/zijing16/"

output_name="screen.avi"
./build/player_standalone/player_standalone $mode $path $output_name
