/*
 * InstUtility.hpp
 *
 *  Created on: 2016/03/10
 *      Author: LittleBird
 */

#ifndef COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTUTILITY_HPP_
#define COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTUTILITY_HPP_

#include <cstring>
#include <sstream>
#include <string>
#include "InstType.hpp"

namespace inst {

struct AssemblerArgumentInfo {
    bool useHex;
    bool hasA, hasD;
    bool hasInputFile;
    bool hasOutputFile;
    bool hasNoLabel;
    bool hasInitPc;
    bool isValid;
    int initPc;
    std::string inputFile;
    std::string outputFile;

    AssemblerArgumentInfo() {
        useHex = hasA = hasD = hasInputFile = hasOutputFile = hasNoLabel = hasInitPc = isValid = false;
        initPc = 0;
        inputFile = "";
        outputFile = "";
    }
};

template<typename Tp>
std::string toString(const Tp& val) {
    std::basic_stringstream<char> oss;
    oss << val;
    return oss.str();
}

// if has number, use decimal
template<typename Tp>
std::string toDecString(const Tp& val) {
    std::basic_stringstream<char> oss;
    oss << val;
    return oss.str();
}

// if has number, use hex-decimal
template<typename Tp>
std::string toHexString(const Tp& val) {
    std::basic_stringstream<char> oss;
    oss << std::showbase << std::hex << val;
    return oss.str();
}

// write an unsigned int to file(big endian)
int fwriteUnsigned(FILE* fout, const unsigned& src);

// check main arguments for easy assembler
bool isValidArguments(int& argc, char**& argv);

// analyze main arguments
AssemblerArgumentInfo processArguments(int& argc, char**& argv);

// extend sign bits
// similar to int toSigned(const unsigned& src, const LB::InstMemLen& type);
// argument type pass by bits number instead
// bits: src bits
int toSigned(const unsigned& src, const int& bits);

// for convenience, only use static_cast<unsigned>
unsigned toUnsigned(const int& src);

// get bits from range [l, r)
// ex. getBitsInRange(0xC, 0, 3) returns 0x4
// 0xC = 0b1100 returns 0b100
// zero based
unsigned getBitsInRange(const unsigned& src, const int& l, const int& r);

} /* namespace inst */

#endif /* COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTUTILITY_HPP_ */
