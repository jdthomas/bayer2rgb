#!/bin/sh

echo "Trying on 8 bit image"
for M in NEAREST SIMPLE BILINEAR HQLINEAR DOWNSAMPLE EDGESENSE VNG AHD; do
	time bayer2rgb -i SV0030.RAW -o SV0030.$M.tif -w 1328 -v 498 -b 8 -m $M -t
done
time convert -size 1328x498  -depth 8  RGGB_BAYER:./SV0030.RAW ./SV0030.png

echo "Trying on 16 bit image"
for M in NEAREST SIMPLE BILINEAR HQLINEAR DOWNSAMPLE EDGESENSE VNG AHD; do
    time bayer2rgb -i ./SV0060.RAW -o ./SV0060.$M.tif -w 3696 -v 2784 -b 16 -f BGGR -m $M -t
done
time convert -size 3696x2784 -depth 16 RGGB_BAYER:./SV0060.RAW ./SV0060.png

exit 0
echo "Trying on other endian image"
for M in NEAREST SIMPLE BILINEAR HQLINEAR DOWNSAMPLE EDGESENSE VNG AHD; do
    time bayer2rgb -i ./SV0090.RAW -o ./SV0090.$M.tif -w 3072 -v 2304 -b 16 -f BGGR -m $M -t -s
done
time convert -size 3072x2304 -depth 16 BGGR_BAYER:./SV0090.RAW ./SV0090.png

