CXXFLAGS = -O2 -pipe -Wall
OBJ = main.o shader.o image.o deps/stb_truetype.o window.o deps/glad.o

main.exe: $(OBJ)
	g++ $(OBJ) -o main.exe -lglfw3 -lopengl32 -lgdi32 -s

main.o: main.cpp shader.h image.h window.h utf8.h
	g++ -c main.cpp -o main.o $(CXXFLAGS)

deps/glad.o: deps/glad.c deps/glad.h
	g++ -c deps/glad.c -o deps/glad.o $(CXXFLAGS)

shader.o: shader.cpp shader.h
	g++ -c shader.cpp -o shader.o $(CXXFLAGS)

image.o: image.cpp image.h
	g++ -c image.cpp -o image.o $(CXXFLAGS)

deps/stb_truetype.o: deps/stb_truetype.cpp deps/stb_truetype.h
	g++ -c deps/stb_truetype.cpp -o deps/stb_truetype.o $(CXXFLAGS)

window.o: window.cpp window.h
	g++ -c window.cpp -o window.o $(CXXFLAGS)
