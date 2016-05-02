CC := g++

CXXFLAGS := -std=c++11 -Wall -Wextra -Os

OBJS := InstAssembler.o \
        InstDataBin.o \
        InstDataStr.o \
        InstDecoder.o \
        InstDisassembler.o \
        InstEncoder.o \
        InstImageReader.o \
        InstLookUp.o \
        InstUtility.o \
        main.o

.SUFFIXS:
.SUFFIXS: .cpp .o

.PHONY: clean

all: assembler

assembler: ${OBJS}
	${CC} ${CXXFLAGS} ${OBJS} -o $@

.cpp.o:
	${CC} ${CXXFLAGS} -c $<

clean:
	-rm -f *.o assembler
