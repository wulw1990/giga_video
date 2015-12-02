mode="image"

path="/home/wuliwei/dataset/bbnc/giga_image/"
info_file=$path"info_scene.txt"

./build/player_standalone/player_standalone $mode $path $info_file
