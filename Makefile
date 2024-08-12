main.exe: main.cpp shader.o image.o deps/stb_truetype.o
	g++ main.cpp shader.o image.o deps/stb_truetype.o -o main.exe -O2 -pipe -Wall -lglfw3 -lglew32 -lopengl32 -lgdi32 -s

shader.o: shader.cpp shader.h
	g++ -c shader.cpp -o shader.o -O2 -pipe -Wall

image.o: image.cpp image.h
	g++ -c image.cpp -o image.o -O2 -pipe -Wall

deps/stb_truetype.o: deps/stb_truetype.cpp deps/stb_truetype.h
	g++ -c deps/stb_truetype.cpp -o deps/stb_truetype.o -O2 -pipe -Wall
