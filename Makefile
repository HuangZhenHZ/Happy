CXXFLAGS = -O2 -pipe -Wall
OBJ = main.o shader.o image.o deps/stb_truetype.o window.o deps/glad.o

main.exe: $(OBJ)
	g++ $(OBJ) -o main.exe -lglfw3 -lopengl32 -lgdi32 -s

%.d: %.cpp %.o
	g++ -MM $< -o $@ -MT $(@:.d=.o) $(CXXFLAGS)

include $(OBJ:.o=.d)
