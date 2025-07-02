CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -g

BUILD_DIR = build

TARGET = grep
TARGET_PATH = $(BUILD_DIR)/$(TARGET)

all: $(TARGET_PATH)

$(BUILD_DIR):
	mkdir -p $@

$(TARGET_PATH): src/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $<

run: $(TARGET_PATH)
	./$(TARGET_PATH)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean run
