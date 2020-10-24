# instal glut

all: main.cpp
	make clean
	g++ main.cpp -o main -Iinclude/ -lGL -lGLU -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi


.PHONY: clean
clean:
	rm main	
