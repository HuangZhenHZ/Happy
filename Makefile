main.exe: main.cpp shader.o image.o
	g++ main.cpp shader.o image.o -o main.exe -O2 -pipe -Wall -lglfw3 -lglew32 -lopengl32 -lgdi32 -s

shader.o: shader.cpp shader.h
	g++ -c shader.cpp -o shader.o -O2 -pipe -Wall

image.o: image.cpp image.h
	g++ -c image.cpp -o image.o -O2 -pipe -Wall
