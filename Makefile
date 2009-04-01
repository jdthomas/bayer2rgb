
ifdef DEBUG
  CFLAGS += -ggdb -Wall -DDEBUG
else
  CFLAGS += -O3 -Wall
endif


all: bayer2rgb
	
bayer2rgb: bayer2rgb.c bayer.o
	$(CC) $(CFLAGS) -o bayer2rgb bayer.o bayer2rgb.c -lm
ifdef RELEASE
	strip bayer2rgb
endif

bayer.o:bayer.c
	$(CC) $(CFLAGS) -c bayer.c

clean:
	-rm bayer2rgb bayer.o *\~ 
