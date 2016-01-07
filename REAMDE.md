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