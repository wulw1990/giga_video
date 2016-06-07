## Build
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
Edit CMakeLists.txt, if you have FlyCapture driver installed :
```
option (FLY_CAPTRUE "Use provided math implementation" ON)
```
Otherwise:
```
option (FLY_CAPTRUE "Use provided math implementation" OFF)
```
```
make
make clean
```
## Run
some example scripts in samples:
```
../samples/demo.sh
../samples/demo_slave.sh
```
