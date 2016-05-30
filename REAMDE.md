## 编译项目
```
sudo apt-get install cmake
sudo apt-get install libopencv-dev
```

ubuntu 14.04
```
sudo add-apt-repository --yes ppa:xqms/opencv-nonfree
sudo apt-get update 
sudo apt-get install libopencv-nonfree-dev
```
```
make
make clean
../samples/demo.sh
```
## architecture

![architecture](readme/architecture_hardware.jpg)


### data acquisition system

one master with multi slave

each slave has multi camera

### application

now mobile(android) supported


## TODO
construct camera set

giga show camera set

transmit between master and slave

## 选择是否编译FlyCapture
如果不在采集设备上运行，编译项目会出现FlyCapture的编译错误。

编辑最外层目录的CMakeLists.txt

如果启用FlyCapture：

```
if(1)
	add_definitions(-DENABLE_FLY_CAPTRUE)
```

否则:

```
if(0)
	add_definitions(-DENABLE_FLY_CAPTRUE)
```

## 肖赟demo的构建和浏览
编辑./sh/construct.sh 调用mode="construct_xiaoyun"，设置对应的参数

运行./sh/construct.sh 构建，程序会把每一帧切割好，按固定格式保存。

编辑./sh/player_xiaoyun.sh 设置对应的参数

运行./sh/plyaer_xiaoyun.sh 浏览。双击放大，右键双击缩小。按下左键拖动表示移动画面。

## 静态全景图的构建和浏览
1. 使用gigapan+单反相机采集全景图。

2. 使用kolor软件进行拼接和分割成kolor默认的存储格式（金字塔形状，切割成很多小的tile）

3. 编辑./sh/construct.sh 调用mode="construct_from_autopan"，设置对应的参数。
其中：path表示存放小tile的路径，里面有0,1,2...文件夹。n_layers表示层的总数，head_layers表示前head_layers层的存储结构和后面的不同，可打开各个层观察。
如果运行成功，会在path路径下生成一个info_scene.txt

4. 编辑./sh/player_standalone, mode设置为image，设置其他对应参数

5. 运行./sh/player_standalone 浏览。双击放大，右键双击缩小。按下左键拖动表示移动画面。


## 录制摄像头视频-单机模式
编辑./sh/camera.sh 设置mode="record" 设置对应参数

运行脚本./sh/camera.sh 进行录制

## 录制摄像头视频-多机模式
选择其中一台机器作为master，其他作为slave

在master机器上，编辑./sh/record_master.sh 设置对应参数

运行脚本./sh/record_master.sh

在slave机器上，编辑./sh/record_slave.sh 设置对应参数

运行脚本./sh/record_slave.sh 

./sh/record_master.sh中制定了slave的数量，当所有slave连接到master后，各个机器分别开始录制。


## 数据存储格式
```
giga_video/
 - info_scene.txt
 - 0/
 - 1/
 - ...
 - video/
    - list.txt
 	- <NAME1>/
	 	- info.txt
		- video.avi
 	- <NAME2>/
	 	- info.txt
		- video.avi
	- ...
```
info.txt:
```
2.11394	-0.0160635	17.728	
-0.0149863	2.1264	29.119	
-3.59669e-06	-9.43491e-06	1	
71898	6046	4130	2350	
```


## 实时视频流的配置
```
http://blog.csdn.net/xmzwlw/article/details/51458762
```
