/*
 * InstImageReader.hpp
 *
 *  Created on: 2016/03/24
 *      Author: LittleBird
 */

#ifndef COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTIMAGEREADER_HPP_
#define COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTIMAGEREADER_HPP_

#include <cstdio>
#include <cstdlib>
#include <string>

namespace inst {

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

} /* namespace inst */

#endif /* COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTIMAGEREADER_HPP_ */
