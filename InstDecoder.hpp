/*
 * InstDecoder.hpp
 *
 *  Created on: 2016/03/09
 *      Author: LittleBird
 */

#ifndef COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTDECODER_HPP_
#define COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTDECODER_HPP_

#include <string>
#include "InstDataBin.hpp"
#include "InstDataStr.hpp"
#include "InstLookUp.hpp"
#include "InstUtility.hpp"
#include "InstType.hpp"

namespace inst {

class InstDecoder {
public:
    // Decode hex-decimal format instruction set to readable string
    // All static functions
    // return type InstData defined in "InstData.h"

    // decode instruction set, saved by string
    static InstDataStr decodeInstStr(const unsigned& src);

    // decode instruction set, saved by string, add arguments for use dec or hex on constant
    static InstDataStr decodeInstStr(const unsigned& src, const bool& useHex);

    // decode instruction set, save by unsigned
    static InstDataBin decodeInstBin(const unsigned& src);

};

} /* namespace inst */

#endif /* COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTDECODER_HPP_ */
