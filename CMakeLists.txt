cmake_minimum_required(VERSION 3.5)

project(Computer_Architecture_Easy_Assembler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -Wall -Wextra")

set(SOURCE_FILES
        InstAssembler.cpp
        InstAssembler.h
        InstDataBin.cpp
        InstDataBin.h
        InstDataStr.cpp
        InstDataStr.h
        InstDecoder.cpp
        InstDecoder.h
        InstDisassembler.cpp
        InstDisassembler.h
        InstEncoder.cpp
        InstEncoder.h
        InstImageReader.cpp
        InstImageReader.h
        InstLookUp.cpp
        InstLookUp.h
        InstType.h
        InstUtility.cpp
        InstUtility.h
        main.cpp)

add_executable(assembler ${SOURCE_FILES})
