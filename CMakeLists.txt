cmake_minimum_required(VERSION 3.5)

project(Computer_Architecture_Easy_Assembler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -Wall -Wextra")

set(SOURCE_FILES
        InstAssembler.cpp
        InstAssembler.hpp
        InstDataBin.cpp
        InstDataBin.hpp
        InstDataStr.cpp
        InstDataStr.hpp
        InstDecoder.cpp
        InstDecoder.hpp
        InstDisassembler.cpp
        InstDisassembler.hpp
        InstEncoder.cpp
        InstEncoder.hpp
        InstImageReader.cpp
        InstImageReader.hpp
        InstLookUp.cpp
        InstLookUp.hpp
        InstType.hpp
        InstUtility.cpp
        InstUtility.hpp
        main.cpp)

add_executable(assembler ${SOURCE_FILES})
