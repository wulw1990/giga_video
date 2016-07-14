mode="demo"
mode_internal="server"
#path="${HOME}/wuliwei/data/zijing16_1/"
path="${HOME}/wuliwei/data/main_building/"
mode_video="2"
port_slave=5000
port_client=5001
./build/giga_video $mode $mode_internal $path $mode_video $port_slave $port_client
