
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


2. brew and opencv, cfitsio 설치 (Mac OS)
----------

LaunchPad 에서 유틸리티/터미널 실행한 후 
		
	# brew install
	/usr/bin/ruby -e "$(/usr/bin/curl -fsSL https://raw.github.com/mxcl/homebrew/master/Library/Contributions/install_homebrew.rb)"
		
	# install opencv
	brew update
	brew install opencv cfitsio


2-1. opencv 설치 (Linux : Debian 6)
-----

리눅스(데비안)에서 opencv2.1 을 설치하려면, 다음과 같이 하면되는데,

	sudo apt-get install libcv-dev libhighgui-dev  libcvaux-dev libcfitsio3-dev

opencv 2.1에서는 간혹 메모리 오류가 발생하기 때문에...

( [OpenCV on Debian](http://www.lengrand.fr/2011/11/compiling-opencv-for-linux-debian/)와
[OpenCV Wiki](http://opencv.willowgarage.com/wiki/InstallGuide%20%3A%20Debian) 참고해서 컴파일하는 것이 안정적이다.

* 저 페이지의 svn은 동작안함, http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/2.3.1/에서 소스 다운로드해야함

* 중간에 v4l-dev는 libv4l-dev

* CMake 부분은 아래처럼
	cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D BUILD_PYTHON_SUPPORT=ON -D BUILD_EXAMPLES=ON /home/jinto/OpenCV-2.3.1/



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

	# make만 실행하면 됨
	# 실행할 때는  옵션으로 fits파일을 지정해야함, fits 파일은 samples폴더에 있어야함.
	./f2j 20120308_edit_00.fit 
		

Todo
====
