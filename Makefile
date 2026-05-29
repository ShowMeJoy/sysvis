CC       = gcc
CXX      = g++
CFLAGS   = -Wall -Wextra -std=c17 -Isrc -O2
CXXFLAGS = -Wall -std=c++17 -O2 -fno-exceptions -fno-rtti
TARGET   = tracer
IMGUI_DIR = third_party/cimgui

SDL2_CFLAGS = $(shell pkg-config --cflags sdl2)
SDL2_LIBS   = $(shell pkg-config --libs sdl2)

CFLAGS   += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/imgui -I$(IMGUI_DIR)/imgui/backends $(SDL2_CFLAGS)
CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_DIR)/imgui -I$(IMGUI_DIR)/imgui/backends $(SDL2_CFLAGS)

DEFINES = -DCIMGUI_USE_SDL2 -DCIMGUI_USE_OPENGL3 -DIMGUI_IMPL_API='extern "C" '
CFLAGS += -DCIMGUI_USE_SDL2 -DCIMGUI_USE_OPENGL3

SRC_C = src/main.c src/process.c src/syscalls.c src/tracer.c src/eventbuf.c src/gui/gui.c
SRC_CPP = $(IMGUI_DIR)/cimgui_impl.cpp \
          $(IMGUI_DIR)/imgui/backends/imgui_impl_sdl2.cpp \
          $(IMGUI_DIR)/imgui/backends/imgui_impl_opengl3.cpp

OBJ_C   = $(SRC_C:.c=.o)
OBJ_CPP = $(SRC_CPP:.cpp=.o)

LIBS = -L$(IMGUI_DIR) -lcimgui $(SDL2_LIBS) -lGL -lstdc++

all: build

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEFINES) -c -o $@ $<

build: $(OBJ_C) $(OBJ_CPP)
	$(CXX) -o $(TARGET) $(OBJ_C) $(OBJ_CPP) $(LIBS)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJ_C) $(OBJ_CPP)