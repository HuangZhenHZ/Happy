CXXFLAGS = -O2 -pipe -Wall
SRCS = main.cpp shader.cpp image.cpp deps/stb_truetype.cpp window.cpp deps/glad.cpp font.cpp
BUILD_DIR = build/
OBJS = $(addprefix $(BUILD_DIR), $(SRCS:.cpp=.o))
DIRS_WITH_SLASH = $(sort $(dir $(OBJS)))
DIRS = $(DIRS_WITH_SLASH:/=)

main.exe: $(OBJS)
	g++ $(OBJS) -o main.exe -lglfw3 -lopengl32 -lgdi32 -s

$(DIRS):
	mkdir $(subst /,\,$@)

$(BUILD_DIR)%.d $(BUILD_DIR)%.o : %.cpp | $(DIRS)
	g++ -c $< -o $(BUILD_DIR)$(<:.cpp=.o) -MMD -MT $(BUILD_DIR)$(<:.cpp=.o) -MP $(CXXFLAGS)

include $(OBJS:.o=.d)
