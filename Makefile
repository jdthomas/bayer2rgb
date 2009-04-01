
all: Bayer2RGB
	
Bayer2RGB: Bayer2RGB.c
	gcc -g -o Bayer2RGB Bayer2RGB.c

bayer.o:bayer.c
	gcc -g -c bayer.c

clean:
	-rm Bayer2RGB
