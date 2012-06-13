all: f2j line

UNAME := $(shell uname)


ifeq ($(UNAME), Linux)
	CC = gcc
endif
ifeq ($(UNAME), Darwin)
	CC = llvm-g++
endif

f2j: dtobj.cpp jpegsubs.cpp
	$(CC) `pkg-config --libs --cflags opencv` -ljpeg -lcfitsio dtobj.cpp jpegsubs.cpp -o f2j

line: line.c
	$(CC)  `pkg-config --libs --cflags opencv` line.c -o line

#libcfitsio.a: cfitsio/*.c
	#cd cfitsio; make;cd ..
	#cd cfitsio; ./configure; make;cd ..


clean:
	rm *.o
