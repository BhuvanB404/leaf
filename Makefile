CC ?= g++
CFLAGS ?= -std=c++17 -O2 -Wall -Wextra -I src
TARGET ?= leaf
SRC ?= src/main.cpp
ASM ?= out.asm
OUT ?= out

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: all
	@if [ -f example.hy ]; then \
		./$(TARGET) example.hy || true; \

	fi

clean:
	rm -f $(TARGET) $(OUT) $(OUT).o $(ASM)
