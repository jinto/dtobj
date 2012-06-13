all: f2j line

f2j: cfitsio/libcfitsio.a dtobj.cpp jpegsubs.c
	llvm-g++  `pkg-config --libs --cflags opencv` -ljpeg cfitsio/libcfitsio.a dtobj.cpp jpegsubs.c -o f2j

line: line.c
	llvm-g++  `pkg-config --libs --cflags opencv` line.c -o line

#libcfitsio.a: cfitsio/*.c
	#cd cfitsio; make;cd ..
	#cd cfitsio; ./configure; make;cd ..

