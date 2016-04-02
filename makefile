CC := g++
CFLAGS := -std=c++11 -Wall -O3
OBJS := InstDataBin.o InstDataStr.o InstDecoder.o \
        InstEncoder.o InstImageReader.o InstLookUp.o \
        InstUtility.o main.o

.SUFFIXS:
.SUFFIXS: .cpp .o
.PHONY: clean

all: assembler

assembler: ${OBJS}
	${CC} ${CFLAGS} ${OBJS} -o $@

.cpp.o:
	${CC} ${CFLAGS} -c $<

clean:
	-rm -f *.o assembler
