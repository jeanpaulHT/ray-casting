all: compile execute

compile:
	g++ main.cpp -I/usr/include/opencv4/ -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs 

execute:
	./a.out