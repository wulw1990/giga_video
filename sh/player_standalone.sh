mode="image"

# path="/home/wuliwei/dataset/bbnc/zijing16/"
path="/media/wuliwei/data/NB_BBNC/giga_video/zijing16/"
info_file=$path"info_scene.txt"

./build/player_standalone/player_standalone $mode $path $info_file
