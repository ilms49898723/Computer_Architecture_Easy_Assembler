/*
 * InstDecode.h
 *
 *  Created on: 2016/03/09
 *      Author: LittleBird
 */

#ifndef INSTDECODE_H_
#define INSTDECODE_H_

#include <string>
#include "InstDataBin.h"
#include "InstDataStr.h"
#include "InstLookUp.h"
#include "InstUtility.h"
#include "InstEnum.h"

namespace lb {

class InstDecode {
public:
    // Decode hex-decimal format instruction set to readable string
    // All static functions
    // return type InstData defined in "InstData.h"

    // decode instruction set, saved by string
    static InstDataStr decodeInstStr(const unsigned& src);
    // decode instruction set
    // argument array passed by pointer(array length 2)
    // will be merged and call decodeInstStr(const unsigned &src);
    static InstDataStr decodeInstStr(const unsigned* src);

    // decode instruction set, save by unsigned
    static InstDataBin decodeInstBin(const unsigned& src);
    // decode instruction set
    // argument array passed by pointer(array length 2)
    // will be merged and call decodeInstBin(const unsigned &src);
    static InstDataBin decodeInstBin(const unsigned* src);
};

} /* namespace lb */

#endif /* INSTDECODE_H_ */
