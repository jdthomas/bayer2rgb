#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/**
 *
 * Attributes that can be passed by ImageMagick
 *
 *  %i  input image filename
 *  %o  output image filename
 *  %u  unique temporary filename
 *  %z  secondary unique temporary filename
 *  %#  input image signature
 *  %b  image file size
 *  %c  input image comment
 *  %g  image geometry
 *  %h  image rows (height)
 *  %k  input image number colors
 *  %m  input image format
 *  %p  page number
 *  %q  input image depth
 *  %s  scene number
 *  %w  image columns (width)
 *  %x  input image x resolution
 *  %y  input image y resolution
 *
 *  ./Bayer2RGB -i %i -o %o -w %x -h %y -b %q
 */

/**
 * RAW sensor filters. These elementary tiles tesselate the image plane in RAW modes. RGGB should be interpreted in 2D as
 *
 *    RG
 *    GB
 *
 * and similarly for other filters.
 */
typedef enum {
    COLOR_FILTER_RGGB = 512,
    COLOR_FILTER_GBRG,
    COLOR_FILTER_GRBG,
    COLOR_FILTER_BGGR
} color_filter_t;
#define COLOR_FILTER_MIN        COLOR_FILTER_RGGB
#define COLOR_FILTER_MAX        COLOR_FILTER_BGGR
#define COLOR_FILTER_NUM       (COLOR_FILTER_MAX - COLOR_FILTER_MIN + 1)

#define CLIP(in, out)\
   in = in < 0 ? 0 : in;\
   in = in > 255 ? 255 : in;\
   out=in;

#define CLIP16(in, out, bits)\
   in = in < 0 ? 0 : in;\
   in = in > ((1<<bits)-1) ? ((1<<bits)-1) : in;\
   out=in;

void
ClearBorders(uint8_t *rgb, int sx, int sy, int w)
{
    int i, j;
    // black edges are added with a width w:
    i = 3 * sx * w - 1;
    j = 3 * sx * sy - 1;
    while (i >= 0) {
        rgb[i--] = 0;
        rgb[j--] = 0;
    }

    int low = sx * (w - 1) * 3 - 1 + w * 3;
    i = low + sx * (sy - w * 2 + 1) * 3;
    while (i > low) {
        j = 6 * w;
        while (j > 0) {
            rgb[i--] = 0;
            j--;
        }
        i -= (sx - 2 * w) * 3;
    }
}

void
ClearBorders_uint16(uint16_t * rgb, int sx, int sy, int w)
{
    int i, j;

    // black edges:
    i = 3 * sx * w - 1;
    j = 3 * sx * sy - 1;
    while (i >= 0) {
        rgb[i--] = 0;
        rgb[j--] = 0;
    }

    int low = sx * (w - 1) * 3 - 1 + w * 3;
    i = low + sx * (sy - w * 2 + 1) * 3;
    while (i > low) {
        j = 6 * w;
        while (j > 0) {
            rgb[i--] = 0;
            j--;
        }
        i -= (sx - 2 * w) * 3;
    }

}

/* OpenCV's Bayer decoding */
int
bayer_Bilinear(const uint8_t * bayer, uint8_t * rgb, int sx, int sy, int tile)
{
    const int bayerStep = sx;
    const int rgbStep = 3 * sx;
    int width = sx;
    int height = sy;
    /*
       the two letters  of the OpenCV name are respectively
       the 4th and 3rd letters from the blinky name,
       and we also have to switch R and B (OpenCV is BGR)

       CV_BayerBG2BGR <-> COLOR_FILTER_BGGR
       CV_BayerGB2BGR <-> COLOR_FILTER_GBRG
       CV_BayerGR2BGR <-> COLOR_FILTER_GRBG

       int blue = tile == CV_BayerBG2BGR || tile == CV_BayerGB2BGR ? -1 : 1;
       int start_with_green = tile == CV_BayerGB2BGR || tile == CV_BayerGR2BGR;
     */
    int blue = tile == COLOR_FILTER_BGGR || tile == COLOR_FILTER_GBRG ? -1 : 1;
    int start_with_green = tile == COLOR_FILTER_GBRG || tile == COLOR_FILTER_GRBG;

    fprintf(stderr, "***Here: %s:%d\n",__FILE__,__LINE__);

    if ((tile>COLOR_FILTER_MAX)||(tile<COLOR_FILTER_MIN))
    {
        printf("Invalid color filter: %d\n", tile);
        return -1; //INVALID_COLOR_FILTER;
    }

    fprintf(stderr, "***Here: %s:%d\n",__FILE__,__LINE__);
    ClearBorders(rgb, sx, sy, 1);
    fprintf(stderr, "***Here: %s:%d\n",__FILE__,__LINE__);
    rgb += rgbStep + 3 + 1;
    height -= 2;
    width -= 2;

    for (; height--; bayer += bayerStep, rgb += rgbStep) {
        int t0, t1;
        const uint8_t *bayerEnd = bayer + width;

        if (start_with_green) {
            /* OpenCV has a bug in the next line, which was
               t0 = (bayer[0] + bayer[bayerStep * 2] + 1) >> 1; */
            t0 = (bayer[1] + bayer[bayerStep * 2 + 1] + 1) >> 1;
            t1 = (bayer[bayerStep] + bayer[bayerStep + 2] + 1) >> 1;
            rgb[-blue] = (uint8_t) t0;
            rgb[0] = bayer[bayerStep + 1];
            rgb[blue] = (uint8_t) t1;
#if 0
            printf("%p %d %d\n", &rgb[-blue], t0, rgb[-blue]);
            printf("%p %d %d\n", rgb, bayer[bayerStep + 1], rgb[0]);
            printf("%p %d %d\n", &rgb[blue], t1, rgb[blue]);
#endif
            bayer++;
            rgb += 3;
        }

        if (blue > 0) {
            for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
                t0 = (bayer[0] + bayer[2] + bayer[bayerStep * 2] +
                      bayer[bayerStep * 2 + 2] + 2) >> 2;
                t1 = (bayer[1] + bayer[bayerStep] +
                      bayer[bayerStep + 2] + bayer[bayerStep * 2 + 1] +
                      2) >> 2;
                rgb[-1] = (uint8_t) t0;
                rgb[0] = (uint8_t) t1;
                rgb[1] = bayer[bayerStep + 1];
#if 0
                printf("%p %d %d\n", &rgb[-1], t0, rgb[-1]);
                printf("%p %d %d\n", rgb, bayer[bayerStep + 1], rgb[0]);
                printf("%p %d %d\n", &rgb[1], t1, rgb[1]);
#endif
                t0 = (bayer[2] + bayer[bayerStep * 2 + 2] + 1) >> 1;
                t1 = (bayer[bayerStep + 1] + bayer[bayerStep + 3] +
                      1) >> 1;
                rgb[2] = (uint8_t) t0;
                rgb[3] = bayer[bayerStep + 2];
                rgb[4] = (uint8_t) t1;
#if 0
                printf("%p %d %d\n", &rgb[2], t0, rgb[2]);
                printf("%p %d %d\n", rgb, bayer[bayerStep + 2], rgb[3]);
                printf("%p %d %d\n", &rgb[4], t1, rgb[4]);
#endif
            }
        } else {
            for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
                t0 = (bayer[0] + bayer[2] + bayer[bayerStep * 2] +
                      bayer[bayerStep * 2 + 2] + 2) >> 2;
                t1 = (bayer[1] + bayer[bayerStep] +
                      bayer[bayerStep + 2] + bayer[bayerStep * 2 + 1] +
                      2) >> 2;
                rgb[1] = (uint8_t) t0;
                rgb[0] = (uint8_t) t1;
                rgb[-1] = bayer[bayerStep + 1];

                t0 = (bayer[2] + bayer[bayerStep * 2 + 2] + 1) >> 1;
                t1 = (bayer[bayerStep + 1] + bayer[bayerStep + 3] +
                      1) >> 1;
                rgb[4] = (uint8_t) t0;
                rgb[3] = bayer[bayerStep + 2];
                rgb[2] = (uint8_t) t1;
            }
        }

        if (bayer < bayerEnd) {
            t0 = (bayer[0] + bayer[2] + bayer[bayerStep * 2] +
                  bayer[bayerStep * 2 + 2] + 2) >> 2;
            t1 = (bayer[1] + bayer[bayerStep] +
                  bayer[bayerStep + 2] + bayer[bayerStep * 2 + 1] +
                  2) >> 2;
            rgb[-blue] = (uint8_t) t0;
            rgb[0] = (uint8_t) t1;
            rgb[blue] = bayer[bayerStep + 1];
            bayer++;
            rgb += 3;
        }

        bayer -= width;
        rgb -= width * 3;

        blue = -blue;
        start_with_green = !start_with_green;
    }
    fprintf(stderr, "***Here: %s:%d\n",__FILE__,__LINE__);
    return 0; //DC1394_SUCCESS;
}

/* OpenCV's Bayer decoding */
int
bayer_Bilinear_uint16(const uint16_t * bayer, uint16_t * rgb, int sx, int sy, int tile, int bits)
{
    const int bayerStep = sx;
    const int rgbStep = 3 * sx;
    int width = sx;
    int height = sy;
    int blue = tile == COLOR_FILTER_BGGR
        || tile == COLOR_FILTER_GBRG ? -1 : 1;
    int start_with_green = tile == COLOR_FILTER_GBRG
        || tile == COLOR_FILTER_GRBG;

    if ((tile>COLOR_FILTER_MAX)||(tile<COLOR_FILTER_MIN))
      return -1; //DC1394_INVALID_COLOR_FILTER;

    rgb += rgbStep + 3 + 1;
    height -= 2;
    width -= 2;

    for (; height--; bayer += bayerStep, rgb += rgbStep) {
        int t0, t1;
        const uint16_t *bayerEnd = bayer + width;

        if (start_with_green) {
            /* OpenCV has a bug in the next line, which was
               t0 = (bayer[0] + bayer[bayerStep * 2] + 1) >> 1; */
            t0 = (bayer[1] + bayer[bayerStep * 2 + 1] + 1) >> 1;
            t1 = (bayer[bayerStep] + bayer[bayerStep + 2] + 1) >> 1;
            rgb[-blue] = (uint16_t) t0;
            rgb[0] = bayer[bayerStep + 1];
            rgb[blue] = (uint16_t) t1;
            bayer++;
            rgb += 3;
        }

        if (blue > 0) {
            for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
                t0 = (bayer[0] + bayer[2] + bayer[bayerStep * 2] +
                      bayer[bayerStep * 2 + 2] + 2) >> 2;
                t1 = (bayer[1] + bayer[bayerStep] +
                      bayer[bayerStep + 2] + bayer[bayerStep * 2 + 1] +
                      2) >> 2;
                rgb[-1] = (uint16_t) t0;
                rgb[0] = (uint16_t) t1;
                rgb[1] = bayer[bayerStep + 1];

                t0 = (bayer[2] + bayer[bayerStep * 2 + 2] + 1) >> 1;
                t1 = (bayer[bayerStep + 1] + bayer[bayerStep + 3] +
                      1) >> 1;
                rgb[2] = (uint16_t) t0;
                rgb[3] = bayer[bayerStep + 2];
                rgb[4] = (uint16_t) t1;
            }
        } else {
            for (; bayer <= bayerEnd - 2; bayer += 2, rgb += 6) {
                t0 = (bayer[0] + bayer[2] + bayer[bayerStep * 2] +
                      bayer[bayerStep * 2 + 2] + 2) >> 2;
                t1 = (bayer[1] + bayer[bayerStep] +
                      bayer[bayerStep + 2] + bayer[bayerStep * 2 + 1] +
                      2) >> 2;
                rgb[1] = (uint16_t) t0;
                rgb[0] = (uint16_t) t1;
                rgb[-1] = bayer[bayerStep + 1];

                t0 = (bayer[2] + bayer[bayerStep * 2 + 2] + 1) >> 1;
                t1 = (bayer[bayerStep + 1] + bayer[bayerStep + 3] +
                      1) >> 1;
                rgb[4] = (uint16_t) t0;
                rgb[3] = bayer[bayerStep + 2];
                rgb[2] = (uint16_t) t1;
            }
        }

        if (bayer < bayerEnd) {
            t0 = (bayer[0] + bayer[2] + bayer[bayerStep * 2] +
                  bayer[bayerStep * 2 + 2] + 2) >> 2;
            t1 = (bayer[1] + bayer[bayerStep] +
                  bayer[bayerStep + 2] + bayer[bayerStep * 2 + 1] +
                  2) >> 2;
            rgb[-blue] = (uint16_t) t0;
            rgb[0] = (uint16_t) t1;
            rgb[blue] = bayer[bayerStep + 1];
            bayer++;
            rgb += 3;
        }

        bayer -= width;
        rgb -= width * 3;

        blue = -blue;
        start_with_green = !start_with_green;
    }

    return 0;//DC1394_SUCCESS;
}


int
main( int argc, char ** argv )
{
    uint32_t ulInSize, ulOutSize, ulWidth, ulHeight, ulBpp;
    int first_color = COLOR_FILTER_RGGB;
    char *infile, *outfile;
    int input_fd = 0;
    int output_fd = 0;
    void * pbyBayer = NULL;
    void * pbyRGB = NULL;
    char c;
    int optidx = 0;

    struct option longopt[] = {
        {"input",1,NULL,'i'},
        {"output",1,NULL,'o'},
        {"width",1,NULL,'w'},
        {"height",1,NULL,'h'},
        {"bpp",1,NULL,'b'},
        {"first",1,NULL,'f'},
        {"method",1,NULL,'m'},
        {0,0,0,0}
    };

    while ((c=getopt_long(argc,argv,"i:o:w:h:b:f:",longopt,&optidx)) != -1)
    {
        switch ( c )
        {
            case 'i':
                infile = strdup( optarg );
                break;
            case 'o':
                outfile = strdup( optarg );
                break;
            case 'w':
                ulWidth = strtol( optarg, NULL, 10 );
                break;
            case 'h':
                ulHeight = strtol( optarg, NULL, 10 );
                break;
            case 'b':
                ulBpp = strtol( optarg, NULL, 10 );
                break;
            case 'f':
				//TODO: Parse this
                first_color = COLOR_FILTER_RGGB;
                break;
            case 'm':
				//TODO: implement methods
                break;
            default:
                printf("bad arg\n");
                return 1;
        }
    }
    // arguments: infile outfile width height bpp first_color
    if( infile == NULL || outfile == NULL || ulBpp == 0 || ulWidth == 0 || ulHeight == 0 )
    {
        printf("Bad parameter\n");
        return 1;
    }

    input_fd = open(infile, O_RDONLY);
    if(input_fd <= 0)
    {
        printf("Problem opening input: %s\n", infile);
        return 1;
    }

    output_fd = open(outfile, O_RDWR | O_CREAT | O_TRUNC);
    if(output_fd <= 0)
    {
        printf("Problem opening output: %s\n", outfile);
        return 1;
    }

    ulInSize = lseek(input_fd, 0, SEEK_END );
    lseek(input_fd, 0, 0);

    //ulOutSize = ulWidth * ulHeight * ulBpp * 3;
    ulOutSize = ulWidth * ulHeight * (8 / ulBpp) * 3;
    ftruncate(output_fd, ulOutSize );

    pbyBayer = mmap(NULL, ulInSize, PROT_READ,  MAP_SHARED|MAP_POPULATE, input_fd, 0);
    if( pbyBayer == MAP_FAILED )
    {
        perror("Faild mmaping input");
        return 1;
    }
    pbyRGB = mmap(NULL, ulOutSize, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_POPULATE, output_fd, 0);
    if( pbyRGB == MAP_FAILED )
    {
        perror("Faild mmaping output");
        return 1;
    }
    printf("%p -> %p\n", pbyBayer, pbyRGB);

    printf("%s: %s(%d) %s(%d) %d %d %d\n", argv[0], infile, ulInSize, outfile, ulOutSize, ulWidth, ulHeight, ulBpp );

    //memset(pbyRGB, 0xff, ulOutSize);//return 1;

	switch(ulBpp)
	{
		case 8:
			bayer_Bilinear((uint8_t*)pbyBayer, (uint8_t*)pbyRGB, ulWidth, ulHeight, first_color);
			break;
		case 16:
		default:
			bayer_Bilinear_uint16((uint16_t*)pbyBayer, (uint16_t*)pbyRGB, ulWidth, ulHeight, first_color, ulBpp);
			break;
	}

    munmap(pbyBayer,ulInSize);
    close(input_fd);

    msync(pbyRGB,ulOutSize,MS_INVALIDATE|MS_SYNC);
    munmap(pbyRGB,ulOutSize);
    close(output_fd);

    fprintf(stderr, "***Here: %s:%d\n",__FILE__,__LINE__);
    return 0;
}
