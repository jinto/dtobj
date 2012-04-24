gcc -o f2j -ljpeg fitsio/libcfitsio.a FITS2jpeg.c jpegsubs.c
./f2j -fits 20120308_edit_00.fit -jpeg tt.jpg -quality 100 -max 2500 -min 2300;open tt.jpg 

FLIGrab은 20120308_edit_00.fit 에 대해서 자동으로 2317~2366 구간을 잡아낸다.
아마도, 50씩 10만큼씩 조사하면 어떨까 싶다.


== First ==
cd cfitsio
./configure
make
