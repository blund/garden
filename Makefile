
run:
	gcc main.c things/*.c \
		-g -Wall \
		-Iinclude \
		-lGL -lGLEW -lglfw -lX11 -lpthread -lXrandr -lXi -ldl -lm
	./a.out
