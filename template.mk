CXXFLAGS = -O2 -pipe -Wall -Werror -I.
# -maes -msse4.1 -DNDEBUG
BUILD_DIR = build/
OBJS = $(addprefix $(BUILD_DIR), $(SRCS:.cpp=.o))
DIRS_WITH_SLASH = $(sort $(dir $(OBJS)))
DIRS = $(DIRS_WITH_SLASH:/=)

$(BUILD_DIR)$(TARGET).exe: $(OBJS)
	g++ $(OBJS) -o $@ $(LDFLAGS) -s

.PHONY: run
run: $(BUILD_DIR)$(TARGET).exe
	$(subst /,\,$<)

$(DIRS):
ifeq ($(OS),Windows_NT)
	mkdir $(subst /,\,$@)
else
	mkdir -p $@
endif

$(BUILD_DIR)%.d $(BUILD_DIR)%.o : %.cpp | $(DIRS)
	g++ $(CXXFLAGS) -MMD -MP -c $< -o $(BUILD_DIR)$(<:.cpp=.o)

include $(OBJS:.o=.d)
