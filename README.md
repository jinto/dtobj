Convert fits to jpeg.


Build
=====
    cd cfitsio; ./configure; make;cd ..
    gcc -o f2j -ljpeg cfitsio/libcfitsio.a fits2jpeg.c jpegsubs.c
    ./f2j -fits samples/20120308_edit_00.fit -jpeg output/20120308_edit_00.jpg -quality 100 -max 2500 -min 2300;open output/20120308_edit_00.jpg 

별이 가장 잘보이는 상태
    ./f2j -fits samples/20120308_edit_00.fit -jpeg output/20120308_edit_00.jpg -quality 100 -max 2400 -min 2350;open output/20120308_edit_00.jpg 

라인이 가장 잘 보이는 상태
    ./f2j -fits samples/20120308_edit_00.fit -jpeg output/20120308_edit_00.jpg -quality 100 -max 2350 -min 2300;open output/20120308_edit_00.jpg 

Todo
====
    
FLIGrab은 20120308_edit_00.fit 에 대해서 자동으로 2317~2366 구간을 잡아낸다.
아마도, 50씩 10만큼씩 조사하면 어떨까 싶다.


FakeSat
=======
위성 흉내내기.

Qt가 필요. https://github.com/jinto/dtobj/wiki/Qt-설치 참고

Copyright
=========
See LICENSE file for Fits to jpeg is from Associated 
GPL.
Copyright (C) 1996                                                   */
/*  Associated Universities, Inc. Washington DC, USA.  
