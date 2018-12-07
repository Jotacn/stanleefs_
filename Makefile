COMPILER = gcc -Wall
FILESYSTEM_FILES = main.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o slfs `pkg-config fuse --cflags --libs`

clean:
	rm slfs
