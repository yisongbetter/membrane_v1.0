Demo: system.cpp
	g++  -o cam_sys system.cpp algo_detector.cpp `pkg-config --cflags --libs opencv3.1.0` -I./include -Wl,-rpath=./lib -L./lib -lX11 -lMvCameraControl -lpthread -O2
	

clean:
	rm *.o cam_sys *~ -rf
