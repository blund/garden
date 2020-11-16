# instal glut

all: main.c
	tcc -v main.c glad.c helpers.c -o main \
		-Iinclude/ -lGL -lGLU \
		-lglfw -lX11 -lXxf86vm \
		-lXrandr -lpthread -lXi \
		-lao -lm -ldl

run: all
	./main


.PHONY: clean
clean:
	rm main	
