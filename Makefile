

run:
	gcc main.c thing.c things/waves.c \
		-g -Wall \
		-Iinclude \
		-lGL -lGLEW -lglfw -lX11 -lpthread -lXrandr -lXi -ldl -lm

	./a.out

web:
	/usr/lib/emscripten/emcc emmain.c thing.c things/waves.c -o index.html \
	-s USE_GLFW=3 \
	-s USE_WEBGL2=1 \
	-s FULL_ES3=1 \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s WASM=1 \
	-s ASSERTIONS \
	-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	--preload-file shaders \
	-O2

	/usr/lib/emscripten/emrun --no_browser --port 8080 .
