

run:
	gcc main.c thing.c glad.c  \
		-Wall \
		-Iinclude \
		-lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm
	./a.out

setup:
	curl --output glad.zip https://glad.dav1d.de/#language=c&specification=gl&api=gl%3D4.0&api=gles1%3Dnone&api=gles2%3Dnone&api=glsc2%3Dnone&profile=core&loader=on
	unzip glad.zip
	rm glad.zip

