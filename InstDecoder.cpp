/*
 * InstDecode.cpp
 *
 *  Created on: 2016/03/09
 *      Author: LittleBird
 */

#include "InstDecoder.hpp"

namespace inst {

InstDataStr InstDecoder::decodeInstStr(const unsigned& src) {
    unsigned opCode;
    unsigned rs, rt, rd;
    unsigned c;
    unsigned funct;
    std::string opCodeStr;
    std::string rsStr, rtStr, rdStr;
    std::string cStr;
    std::string functStr;
    opCode = getBitsInRange(src, 26, 32);
    opCodeStr = InstLookUp::opCodeLookUp(opCode);
    if (opCodeStr == "undef") {
        return InstDataStr();
    }
    else if (opCode == 0x0u) {
        funct = getBitsInRange(src, 0, 6);
        rs = getBitsInRange(src, 21, 26);
        rt = getBitsInRange(src, 16, 21);
        rd = getBitsInRange(src, 11, 16);
        c = getBitsInRange(src, 6, 11);
        rsStr = InstLookUp::registerLookUpNumber(rs);
        rtStr = InstLookUp::registerLookUpNumber(rt);
        rdStr = InstLookUp::registerLookUpNumber(rd);
        cStr = toHexString(c);
        functStr = InstLookUp::functLookUp(funct);
        InstDataStr ret;
        ret.setType(InstType::R);
        ret.setOpCode(opCodeStr);
        ret.setRs(rsStr);
        ret.setRt(rtStr);
        ret.setRd(rdStr);
        ret.setC(cStr);
        ret.setFunct(functStr);
        return ret;
    }
    else if (opCode == 0x02u || opCode == 0x03u) {
        c = getBitsInRange(src, 0, 26);
        cStr = toHexString(c);
        InstDataStr ret;
        ret.setType(InstType::J);
        ret.setOpCode(opCodeStr);
        ret.setC(cStr);
        return ret;
    }
    else if (opCode == 0x3Fu) {
        opCodeStr = InstLookUp::opCodeLookUp(opCode);
        InstDataStr ret;
        ret.setType(InstType::S);
        ret.setOpCode(opCodeStr);
        return ret;
    }
    else {
        rs = getBitsInRange(src, 21, 26);
        rt = getBitsInRange(src, 16, 21);
        c = getBitsInRange(src, 0, 16);
        rsStr = InstLookUp::registerLookUpNumber(rs);
        rtStr = InstLookUp::registerLookUpNumber(rt);
        int sc;
        switch (opCode) {
            case 0x08u:
            case 0x23u:
            case 0x21u:
            case 0x25u:
            case 0x20u:
            case 0x24u:
            case 0x2Bu:
            case 0x29u:
            case 0x28u:
            case 0x04u:
            case 0x05u:
            case 0x07u:
                sc = toSigned(c, 16);
                break;
            default:
                sc = static_cast<int>(c);
        }
        cStr = toHexString(sc);
        InstDataStr ret;
        ret.setType(InstType::I);
        ret.setOpCode(opCodeStr);
        ret.setRs(rsStr);
        ret.setRt(rtStr);
        ret.setC(cStr);
        return ret;
    }
}

InstDataStr InstDecoder::decodeInstStr(const unsigned& src, const bool& useHex) {
    unsigned opCode;
    unsigned rs, rt, rd;
    unsigned c;
    unsigned funct;
    std::string opCodeStr;
    std::string rsStr, rtStr, rdStr;
    std::string cStr;
    std::string functStr;
    opCode = getBitsInRange(src, 26, 32);
    opCodeStr = InstLookUp::opCodeLookUp(opCode);
    if (opCodeStr == "undef") {
        return InstDataStr();
    }
    else if (opCode == 0x0u) {
        funct = getBitsInRange(src, 0, 6);
        rs = getBitsInRange(src, 21, 26);
        rt = getBitsInRange(src, 16, 21);
        rd = getBitsInRange(src, 11, 16);
        c = getBitsInRange(src, 6, 11);
        rsStr = InstLookUp::registerLookUpNumber(rs);
        rtStr = InstLookUp::registerLookUpNumber(rt);
        rdStr = InstLookUp::registerLookUpNumber(rd);
        if (useHex) {
            cStr = toHexString(c);
        }
        else {
            cStr = toDecString(c);
        }
        functStr = InstLookUp::functLookUp(funct);
        InstDataStr ret;
        ret.setType(InstType::R);
        ret.setOpCode(opCodeStr);
        ret.setRs(rsStr);
        ret.setRt(rtStr);
        ret.setRd(rdStr);
        ret.setC(cStr);
        ret.setFunct(functStr);
        return ret;
    }
    else if (opCode == 0x02u || opCode == 0x03u) {
        c = getBitsInRange(src, 0, 26);
        if (useHex) {
            cStr = toHexString(c);
        }
        else {
            cStr = toDecString(c);
        }
        InstDataStr ret;
        ret.setType(InstType::J);
        ret.setOpCode(opCodeStr);
        ret.setC(cStr);
        return ret;
    }
    else if (opCode == 0x3Fu) {
        opCodeStr = InstLookUp::opCodeLookUp(opCode);
        InstDataStr ret;
        ret.setType(InstType::S);
        ret.setOpCode(opCodeStr);
        return ret;
    }
    else {
        rs = getBitsInRange(src, 21, 26);
        rt = getBitsInRange(src, 16, 21);
        c = getBitsInRange(src, 0, 16);
        rsStr = InstLookUp::registerLookUpNumber(rs);
        rtStr = InstLookUp::registerLookUpNumber(rt);
        int sc;
        switch (opCode) {
            case 0x08u:
            case 0x23u:
            case 0x21u:
            case 0x25u:
            case 0x20u:
            case 0x24u:
            case 0x2Bu:
            case 0x29u:
            case 0x28u:
            case 0x04u:
            case 0x05u:
            case 0x07u:
                sc = toSigned(c, 16);
                break;
            default:
                sc = static_cast<int>(c);
        }
        if (useHex) {
            cStr = toHexString(sc);
        }
        else {
            cStr = toDecString(sc);
        }
        InstDataStr ret;
        ret.setType(InstType::I);
        ret.setOpCode(opCodeStr);
        ret.setRs(rsStr);
        ret.setRt(rtStr);
        ret.setC(cStr);
        return ret;
    }
}

InstDataBin InstDecoder::decodeInstBin(const unsigned& src) {
    unsigned opCode;
    unsigned rs, rt, rd;
    unsigned c;
    unsigned funct;
    std::string opCodeStr;
    opCode = getBitsInRange(src, 26, 32);
    opCodeStr = InstLookUp::opCodeLookUp(opCode);
    if (opCodeStr == "undef") {
        return InstDataBin();
    }
    else if (opCode == 0x0u) {
        funct = getBitsInRange(src, 0, 6);
        rs = getBitsInRange(src, 21, 26);
        rt = getBitsInRange(src, 16, 21);
        rd = getBitsInRange(src, 11, 16);
        c = getBitsInRange(src, 6, 11);
        InstDataBin ret;
        ret.setInst(src);
        ret.setType(InstType::R);
        ret.setOpCode(opCode);
        ret.setRs(rs);
        ret.setRt(rt);
        ret.setRd(rd);
        ret.setC(c);
        ret.setFunct(funct);
        return ret;
    }
    else if (opCode == 0x02u || opCode == 0x03u) {
        c = getBitsInRange(src, 0, 26);
        InstDataBin ret;
        ret.setInst(src);
        ret.setType(InstType::J);
        ret.setOpCode(opCode);
        ret.setC(c);
        return ret;
    }
    else if (opCode == 0x3Fu) {
        InstDataBin ret;
        ret.setInst(src);
        ret.setType(InstType::S);
        ret.setOpCode(opCode);
        return ret;
    }
    else {
        rs = getBitsInRange(src, 21, 26);
        rt = getBitsInRange(src, 16, 21);
        c = getBitsInRange(src, 0, 16);
        InstDataBin ret;
        ret.setInst(src);
        ret.setType(InstType::I);
        ret.setOpCode(opCode);
        ret.setRs(rs);
        ret.setRt(rt);
        ret.setC(c);
        return ret;
    }
}

} /* namespace inst */
