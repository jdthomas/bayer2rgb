![Actions BuildAndTest Status](https://github.com/jdthomas/bayer2rgb/actions/workflows/cmake.yml/badge.svg)
[![Action Benchmark Status](https://github.com/jdthomas/bayer2rgb/actions/workflows/benchmark.yml/badge.svg)](https://jdthomas.github.io/bayer2rgb/dev/bench/)

# bayer2rgb

> WARNING: Would not recommend this project for anything, it is greater than 13
> years old at this point and not maintained. I am sure you can find better
> projects out there doing essentially the same thing, e.g. OpenCV can demosaic
> raw data. I plan to post some updates just because I wish to toy with cmake
> and github actions and this felt like a good dummy project in which to do
> that. Just becuase there are some recent commits does not mean this is a
> maintained project :) -- Jeff circa 2021

bayer2rgb will convert naked (no header) bayer grid data into rgb data. There
are several choices of interpolation . It can output tiff files, and can
integrate with ImageMagick to output other formats.

The `bayer.c` was borrowed from the libdc1394 project [1], it is licensed under
LGPL. The interpolation algorithms used are described at [3].

This is an implementation of the idea I stumbled upon on ImageMagick's mailing
list while searching for a bayer grid converter. The thread can be found at
[2].

## Building

```bash
mkdir build/ && cd build/
cmake ..  -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
ctest --verbose
```
### Installing

```bash

cp delegates.xml ~/.config/ImageMagick/
```


## Running

    usage: ./bayer2rgb
       --input,-i     input file
       --output,-o    output file
       --width,-w     image width (pixels)
       --height,-v    image height (pixels)
       --bpp,-b       bits per pixel
       --first,-f     first pixel color: RGGB, GBRG, GRBG, BGGR
       --method,-m    interpolation method: NEAREST, SIMPLE, BILINEAR, HQLINEAR, DOWNSAMPLE, EDGESENSE, VNG, AHD
       --tiff,-t      add a tiff header
       --swap,-s      if bpp == 16, swap byte order before conversion
       --help,-h      this helpful message


## Links

1. https://damien.douxchamps.net/ieee1394/libdc1394/
2. http://www.imagemagick.org/pipermail/magick-developers/2008-May/002953.html
3. [http://scien.stanford.edu/class/psych221/projects/99/tingchen/main.htm](https://web.archive.org/web/20090210004353/scien.stanford.edu/class/psych221/projects/99/tingchen/main.htm)


## ImageMagick Integration

A delegates.xml file is included. Add the entries from it to your delegates.xml
and you should be able to do:

convert -size 1328x498 -depth 8 RGGB_BAYER:./tests/SV0030.RAW ./SV0030.jpg

