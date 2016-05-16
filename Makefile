all: 
	mkdir -p build && cd build && cmake .. && make -j16
clean:
	rm -rf build
