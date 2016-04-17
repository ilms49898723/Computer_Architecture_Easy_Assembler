cmake_minimum_required(VERSION 3.5)

project(Computer_Architecture_Easy_Assembler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Os")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -Wall")

set(SOURCE_FILES
    main.cpp
    InstDataBin.cpp
    InstDataBin.h
    InstDataStr.cpp
    InstDataStr.h
    InstDecoder.cpp
    InstDecoder.h
    InstEnum.h
    InstImageReader.cpp
    InstImageReader.h
    InstLookUp.cpp
    InstLookUp.h
    InstUtility.cpp
    InstUtility.h
    InstEncoder.cpp
    InstEncoder.h InstDisassembler.cpp InstDisassembler.h InstAssembler.cpp InstAssembler.h)

add_executable(Computer_Architecture_Easy_Assembler ${SOURCE_FILES})
