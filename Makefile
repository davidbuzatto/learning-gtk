# make build script.
# based on: https://makefiletutorial.com/
#
# usage:
#    make: compile the project and run the compiled file
#    make clean: clean compiled file
#    make cleanAndCompile: clean compiled file and compile the project
#    make compile: compile the project
#    make run: run the compiled file
#
# author: Prof. Dr. David Buzatto

# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
TARGET_EXEC := $(lastword $(notdir $(shell pwd)))

BUILD_DIR := ./build
SRC_DIRS := ./src
PLATFORM := $(shell uname)

all: compile run
compile: $(BUILD_DIR)/$(TARGET_EXEC)
cleanAndCompile: clean compile

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./your_dir/hello.cpp turns into ./build/./your_dir/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# C flags
CFLAGS := $(INC_FLAGS) -IC:/msys64/ucrt64/include/gtk-4.0 -IC:/msys64/ucrt64/include/pango-1.0 -IC:/msys64/ucrt64/include/fribidi -IC:/msys64/ucrt64/include/harfbuzz -IC:/msys64/ucrt64/include/gdk-pixbuf-2.0 -IC:/msys64/ucrt64/include/webp -DLIBDEFLATE_DLL -IC:/msys64/ucrt64/include/cairo -IC:/msys64/ucrt64/include/freetype2 -IC:/msys64/ucrt64/include/libpng16 -IC:/msys64/ucrt64/include/pixman-1 -IC:/msys64/ucrt64/include/graphene-1.0 -IC:/msys64/ucrt64/lib/graphene-1.0/include -mfpmath=sse -msse -msse2 -IC:/msys64/ucrt64/include/glib-2.0 -IC:/msys64/ucrt64/lib/glib-2.0/include -O1 -Wall -Wextra -Wno-unused-parameter -pedantic-errors -std=c99 -Wno-missing-braces

# C++ flags
# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP -O1 -Wall -Wextra -Wno-unused-parameter -pedantic-errors -std=c++20 -Wno-missing-braces

# Linker flags
ifeq ($(PLATFORM), Linux)
LDFLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lgtk-4 -lpangocairo-1.0 -lpangowin32-1.0 -lpango-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lcairo -lharfbuzz -lvulkan-1 -lgraphene-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lintl
else
LDFLAGS := -L lib/ -lgtk-4 -lpangocairo-1.0 -lpangowin32-1.0 -lpango-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lcairo -lharfbuzz -lvulkan-1 -lgraphene-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lintl
endif

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	@rm -f -r $(BUILD_DIR)

.PHONY: run
run:
	./$(BUILD_DIR)/$(TARGET_EXEC)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)