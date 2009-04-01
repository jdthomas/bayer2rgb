echo "Trying on 8 bit image"
for M in NEAREST SIMPLE BILINEAR HQLINEAR DOWNSAMPLE EDGESENSE VNG AHD; do
	time bayer2rgb -i SV0030.RAW -o SV0030.$M.rgb -w 1328 -v 498 -b 8 -m $M
	convert -size 1328x498 -depth 8 RGB:./SV0030.$M.rgb ./SV0030.$M.png
done

echo "Trying on 16 bit image"
for M in NEAREST SIMPLE BILINEAR HQLINEAR DOWNSAMPLE EDGESENSE VNG AHD; do
    time bayer2rgb -i ./SV0060.RAW -o ./SV0060.$M.rgb -w 3696 -v 2784 -b 16 -f BGGR -m $M
    convert -size 3696x2784 -depth 16 -endian LSB RGB:./SV0060.$M.rgb ./SV0060.$M.png
done
