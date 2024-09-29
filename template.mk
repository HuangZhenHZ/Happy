CXXFLAGS = -O3 -pipe -Wall -Werror -maes -msse4.1 -DNDEBUG -I.
BUILD_DIR = build/
OBJS = $(addprefix $(BUILD_DIR), $(SRCS:.cpp=.o))
DIRS_WITH_SLASH = $(sort $(dir $(OBJS)))
DIRS = $(DIRS_WITH_SLASH:/=)

$(TARGET).exe: $(OBJS)
	g++ $(OBJS) -o $(TARGET).exe $(LDFLAGS) -s

.PHONY: run
run:
	$(subst /,\,$(TARGET).exe)

$(DIRS):
ifeq ($(OS),Windows_NT)
	mkdir $(subst /,\,$@)
else
	mkdir $@
endif

$(BUILD_DIR)%.d $(BUILD_DIR)%.o : %.cpp | $(DIRS)
	g++ $(CXXFLAGS) -MMD -MP -c $< -o $(BUILD_DIR)$(<:.cpp=.o)

include $(OBJS:.o=.d)
