CFLAGS=-I/usr/lib/llvm-10/include -std=c++14 -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -gdwarf-4 -Wall
LDFLAGS=-L/usr/lib/llvm-10/lib -lLLVM-10 -lboost_unit_test_framework

all : lljit.exe testsuite.exe

lljit.exe : JITD.h JITD.cpp main.cpp
	${CXX} ${CFLAGS} JITD.cpp main.cpp -o lljit.exe ${LDFLAGS}

testsuite.exe : JITD.h JITD.cpp testsuite.cpp
	${CXX} ${CFLAGS} JITD.cpp testsuite.cpp -o testsuite.exe ${LDFLAGS}

clean :
	-rm lljit.exe testsuite.exe cscope.out TAGS

tags :
	ctags -eR
	cscope -bR
