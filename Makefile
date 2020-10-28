# instal glut

all: main.c
	tcc main.c glad.c -o main \
		-Iinclude/ -lGL -lGLU \
		-lglfw -lX11 -lXxf86vm \
		-lXrandr -lpthread -lXi \
		-lao -lm

run: all
	./main


.PHONY: clean
clean:
	rm main	
