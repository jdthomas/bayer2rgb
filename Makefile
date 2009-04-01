
all: Bayer2RGB
	
Bayer2RGB: Bayer2RGB.c bayer.o
	gcc -g -o Bayer2RGB bayer.o Bayer2RGB.c -lm

bayer.o:bayer.c
	gcc -g -c bayer.c

clean:
	-rm Bayer2RGB bayer.o *\~ 
