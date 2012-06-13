
목적
====
FITS 파일에서 Chopper에 의해 구간별로 쪼개진 직선의 검출. 


(참고 : FITS == http://fits.gsfc.nasa.gov/)

빌드
=====
1. Xcode(command line tool포함) 설치 (Mac OS)
-------

https://developer.apple.com/xcode/ 에서 View in Mac App Store 클릭

설치가 끝난 후에 Xcode 실행하고 메뉴에서 Preferences -> Downloads -> Command Line Tools -> install

![스크린샷](https://github.com/jinto/dtobj/raw/master/xcode_commandline.png)


2. brew and opencv 설치 (Mac OS)
----------

LaunchPad 에서 유틸리티/터미널 실행한 후 
		
	# brew install
	/usr/bin/ruby -e "$(/usr/bin/curl -fsSL https://raw.github.com/mxcl/homebrew/master/Library/Contributions/install_homebrew.rb)"
		
	# install opencv
	brew update
	brew install opencv


2-1. opencv 설치 (Linux : Debian 6)
-----

리눅스(데비안)에서는 다음과 같이 한다.

	sudo apt-get install libcv-dev libhighgui-dev  libcvaux-dev

3. 소스	가져오기
-----

		# 단순 clone
		git clone http://github.com/jinto/dtobj

		# 수정권한이 필요한 경우 (먼저 관리자(jinto)에 요청해서, collaborator에 등록되어야함)
		git clone git@github.com:jinto/dtobj

		# 소스를 받아온 상태에서 최신 버전을 또 가져오려면
		git pull

		# github에서 fork한후 merge 요청하는 방식도 가능

(git 설명서 : http://rogerdudler.github.com/git-guide/index.ko.html)



4. 컴파일
----

		# 최초에는 cfitsio 라이브러리 빌드 필요
		cd cfitsio; ./configure; make;cd ..
		
		# 이후에는 make만 실행하면 됨, 실행할 때는 -fits 옵션으로 fits파일을 지정해야함
		./f2j -fits samples/20120308_edit_00.fit -quality 100 -max 2500 -min 2300;open output/20120308_edit_00.jpg 

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
