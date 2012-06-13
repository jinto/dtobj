
목적
====
FITS 파일에서 Chopper에 의해 구간별로 쪼개진 직선의 검출. (FITS : http://fits.gsfc.nasa.gov/)



빌드(맥OS)
=====
설치할 것들 

1. xcode(command line tool포함)
https://developer.apple.com/xcode/ 에서 View in Mac App Store 클릭

설치가 끝난 후에는 Xcode -> Preferences -> Downloads -> Command Line Tools -> install
		
2. brew and opencv
		
		# brew install
		/usr/bin/ruby -e "$(/usr/bin/curl -fsSL https://raw.github.com/mxcl/homebrew/master/Library/Contributions/install_homebrew.rb)"
		
		# install opencv
		brew update
		brew install opencv

3. 소스	
		git clone 

4. 컴파일

    cd cfitsio; ./configure; make;cd ..;make
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


Copyright
=========
See COPYRIGHT.fits2jpeg Fits to jpeg is from Associated 
GPL.
Copyright (C) 1996                                                   */
/*  Associated Universities, Inc. Washington DC, USA.  
