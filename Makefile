CFLAGS=-I/usr/lib/llvm-9/include -std=c++14 -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -gdwarf-4 -Wall
LDFLAGS=-L/usr/lib/llvm-9/lib -lLLVM-9

lljit.exe : JITD.h JITD.cpp main.cpp
	${CXX} ${CFLAGS} JITD.cpp main.cpp -o lljit.exe ${LDFLAGS}

clean :
	-rm lljit.exe cscope.out TAGS

tags :
	ctags -eR
	cscope -bR
