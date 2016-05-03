/*
 * InstImageReader.cpp
 *
 *  Created on: 2016/03/24
 *      Author: LittleBird
 */

#include "InstImageReader.h"

namespace lb {

unsigned int InstImageReader::readImageI(std::string filePath, unsigned* dst, unsigned* pc, size_t n) {
    FILE* iimage = fopen(filePath.c_str(), "rb");
    if (!iimage) {
        *pc = 0u;
        return 0u;
    }
    unsigned ret = readImageI(iimage, dst, pc, n);
    fclose(iimage);
    return ret;
}

unsigned int InstImageReader::readImageI(FILE* iimage, unsigned* dst, unsigned* pc, size_t n) {
    unsigned readPc = readWordFromBin(iimage);
    unsigned len = readWordFromBin(iimage);
    unsigned wordRead = 0;
    *pc = readPc;
    if (len > n) {
        fprintf(stderr, "Error: buffer to store iimage is not large enough\n");
        exit(EXIT_FAILURE);
    }
    for (wordRead = 0; wordRead < len; ++wordRead) {
        dst[wordRead] = readWordFromBin(iimage);
    }
    return wordRead;
}

unsigned int InstImageReader::readImageD(std::string filePath, unsigned* dst, unsigned* sp, size_t n) {
    FILE* dimage = fopen(filePath.c_str(), "rb");
    if (!dimage) {
        *sp = 0u;
        return 0u;
    }
    unsigned ret = readImageD(dimage, dst, sp, n);
    fclose(dimage);
    return ret;
}

unsigned int InstImageReader::readImageD(FILE* dimage, unsigned* dst, unsigned* sp, size_t n) {
    unsigned readSp = readWordFromBin(dimage);
    unsigned len = readWordFromBin(dimage);
    unsigned wordRead = 0;
    *sp = readSp;
    if (len > n) {
        fprintf(stderr, "Error: buffer to store dimage is not large enough\n");
        exit(EXIT_FAILURE);
    }
    for (wordRead = 0; wordRead < len; ++wordRead) {
        dst[wordRead] = readWordFromBin(dimage);
    }
    return wordRead;
}

unsigned char InstImageReader::readByteFromBin(FILE* fin) {
    unsigned char input;
    if (fread(&input, sizeof(unsigned char), 1, fin) != 1u) {
        return 0u;
    }
    return input;
}

unsigned InstImageReader::readWordFromBin(FILE* fin) {
    unsigned char input[4];
    if (fread(input, sizeof(unsigned char), 4, fin) != 4u) {
        return 0u;
    }
    return (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | (input[3]);
}

} /* namespace lb */
