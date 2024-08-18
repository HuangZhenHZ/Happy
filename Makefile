main.exe: main.o shader.o image.o deps/stb_truetype.o window.o
	g++ main.o shader.o image.o deps/stb_truetype.o window.o -o main.exe -lglfw3 -lglew32 -lopengl32 -lgdi32 -s

main.o: main.cpp shader.h image.h window.h utf8.h
	g++ -c main.cpp -o main.o -O2 -pipe -Wall

shader.o: shader.cpp shader.h
	g++ -c shader.cpp -o shader.o -O2 -pipe -Wall

image.o: image.cpp image.h
	g++ -c image.cpp -o image.o -O2 -pipe -Wall

deps/stb_truetype.o: deps/stb_truetype.cpp deps/stb_truetype.h
	g++ -c deps/stb_truetype.cpp -o deps/stb_truetype.o -O2 -pipe -Wall

window.o: window.cpp window.h
	g++ -c window.cpp -o window.o -O2 -pipe -Wall
