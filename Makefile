SHARED_INCLUDE_DIR = ../shared/
USER_INCLUDE_DIR = ../user/include
CC = clang
LD = ld.lld
CFLAGS = -target i386-elf -std=c23 -m32 -ffreestanding -fno-builtin -O2 -Wall -Wextra -Wpedantic -nostdlib -mno-sse -I $(USER_INCLUDE_DIR) -I $(SHARED_INCLUDE_DIR) -nostdinc -g
LDFLAGS = -m elf_i386 -nostdlib -T ../user.ld

ASSET_DIR = assets
SRCS = $(wildcard *.c) $(wildcard $(ASSET_DIR)/*.c)

OBJS = $(SRCS:.c=.o)

TARGET = sandtris

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)

# Ensure 'clean' is not treated as a file
.PHONY: all clean
