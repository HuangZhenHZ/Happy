main.exe: main.cpp shader.o deps/stb_image.o
	g++ main.cpp shader.o deps/stb_image.o -o main.exe -O2 -pipe -Wall -lglfw3 -lglew32 -lopengl32 -lgdi32 -s

shader.o: shader.cpp shader.h
	g++ -c shader.cpp -o shader.o -O2 -pipe -Wall

deps/stb_image.o: deps/stb_image.cpp deps/stb_image.h
	g++ -c deps/stb_image.cpp -o deps/stb_image.o -O2 -pipe -Wall
