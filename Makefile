SRCS = main.cpp shader.cpp image.cpp deps/stb_truetype.cpp window.cpp deps/glad.cpp font.cpp draw.cpp
TARGET = main
LDFLAGS = -lglfw3 -lopengl32 -lgdi32
include template.mk
