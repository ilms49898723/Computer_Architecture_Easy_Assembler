/*
 * InstImageReader.h
 *
 *  Created on: 2016/03/24
 *      Author: LittleBird
 */

#ifndef INSTIMAGEREADER_H_
#define INSTIMAGEREADER_H_

#include <cstdio>
#include <cstdlib>
#include <string>

namespace lb {

// read iimage.bin, dimage.bin using C I/O
class InstImageReader {
public:
    static unsigned int readImageI(std::string filePath, unsigned* dst, unsigned* pc, size_t n);

    static unsigned int readImageI(FILE* iimage, unsigned* dst, unsigned* pc, size_t n);

    static unsigned int readImageD(std::string filePath, unsigned* dst, unsigned* sp, size_t n);

    static unsigned int readImageD(FILE* dimage, unsigned* dst, unsigned* sp, size_t n);

public:
    static unsigned char readByteFromBin(FILE* fin);

    static unsigned readWordFromBin(FILE* fin);
};

} /* namespace lb */

#endif /* INSTIMAGEREADER_H_ */
