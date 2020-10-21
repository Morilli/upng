UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	CFLAGS := -framework OpenGL
else
	CFLAGS := -lGL
endif

all: png2tga glview

png2tga: png2tga.c upng.c upng.h Makefile
	$(CC) -o png2tga png2tga.c upng.c -Wall -Wextra -pedantic -s -Os -flto -ffunction-sections -fdata-sections -Wl,--gc-sections

glview: glview.c upng.c upng.h Makefile
	$(CC) -o glview glview.c upng.c -Wall -pedantic -g -O3 -flto -lSDL $(CFLAGS)
