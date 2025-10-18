BUILD := build
TARGET := build/floppydot
SOURCE := main.c

CC := gcc
RAYLIB := -lraylib
CFLAGS := $(RAYLIB) -lm -O2 -std=c99 -march=native -mtune=native

.PHONY: all run build clean
all: build

run: $(TARGET)
	./$(TARGET)

build: $(TARGET)

$(TARGET): $(SOURCE)
	@mkdir -p build
	$(CC) -o $(TARGET) $(SOURCE) $(CFLAGS)

clean:
	rm -rf $(BUILD)
