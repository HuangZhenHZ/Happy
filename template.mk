CXXFLAGS = -O2 -pipe -Wall -Werror
BUILD_DIR = build/
OBJS = $(addprefix $(BUILD_DIR), $(SRCS:.cpp=.o))
DIRS_WITH_SLASH = $(sort $(dir $(OBJS)))
DIRS = $(DIRS_WITH_SLASH:/=)

$(TARGET).exe: $(OBJS)
	g++ $(OBJS) -o $(TARGET).exe -lglfw3 -lopengl32 -lgdi32 -s

$(DIRS):
ifeq ($(OS),Windows_NT)
	mkdir $(subst /,\,$@)
else
	mkdir $@
endif

$(BUILD_DIR)%.d $(BUILD_DIR)%.o : %.cpp | $(DIRS)
	g++ -c $< -o $(BUILD_DIR)$(<:.cpp=.o) -MMD -MT $(BUILD_DIR)$(<:.cpp=.o) -MP $(CXXFLAGS)

include $(OBJS:.o=.d)
