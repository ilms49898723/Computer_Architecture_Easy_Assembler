/*
 * InstEncoder.cpp
 *
 *  Created on: 2016/04/12
 *      Author: LittleBird
 */

#include "InstEncoder.h"

namespace lb {

InstEncoder::InstEncoder() {
    init();
}

InstEncoder::~InstEncoder() {
}

void InstEncoder::init() {
    labelTable.clear();
    pc = preprocessPc = 0u;
    line = 1;
}

void InstEncoder::setPc(const unsigned &pc) {
    this->pc = pc;
    this->preprocessPc = pc;
}

void InstEncoder::preProcess(std::string &src) {
    if (isComment(src)) {
        return;
    }
    if (hasLabel(src)) {
        for (unsigned i = 0; i < src.length(); ++i) {
            if (src[i] == ':') {
                std::string label = trimWhiteSpace(src.substr(0, i));
                src = src.substr(i + 1);
                src = trimLeadingWhiteSpace(src);
                labelTable.insert(make_pair(label, preprocessPc));
                preprocessPc += 4;
                return;
            }
        }
    }
    else {
        src = trimLeadingWhiteSpace(src);
        preprocessPc += 4;
        return;
    }
}

InstEncodeData InstEncoder::encodeInst(const std::string& inst) {
    std::string val = inst;
    if (isComment(val)) {
        ++line;
        return InstEncodeData();
    }
    InstEncodeData data = analyzeString(val);
    pc += 4;
    ++line;
    return data;
}

InstEncodeData InstEncoder::analyzeString(std::string inst) {
    inst = processInputString(inst);
    // nop
    if (opToLower(inst) == "nop") {
        return InstEncodeData(0u, true);
    }
    // normal case
    // for instruction operator
    std::string op = nextString(inst);
    InstType instType = getInstType(op);
    if (instType == InstType::R) {
        unsigned funct = InstLookUp::translateToFunct(op);
        if (op == "jr") {
            unsigned rs;
            rs = getReg(nextString(inst));
            unsigned ret = 0u;
            ret |= funct & 0x3Fu;
            ret |= (rs & 0x1Fu) << 21;
            return InstEncodeData(ret, true);
        }
        else if (op == "sll" ||
                 op == "srl" ||
                 op == "sra") {
            unsigned rt, rd, c;
            rd = getReg(nextString(inst));
            rt = getReg(nextString(inst));
            c = getC(nextString(inst));
            unsigned ret = 0u;
            ret |= funct & 0x3Fu;
            ret |= (rt & 0x1Fu) << 16;
            ret |= (rd & 0x1Fu) << 11;
            ret |= (c & 0x1Fu) << 6;
            return InstEncodeData(ret, true);
        }
        else {
            unsigned rs, rt, rd;
            rd = getReg(nextString(inst));
            rs = getReg(nextString(inst));
            rt = getReg(nextString(inst));
            unsigned ret = 0u;
            ret |= funct & 0x3Fu;
            ret |= (rs & 0x1Fu) << 21;
            ret |= (rt & 0x1Fu) << 16;
            ret |= (rd & 0x1Fu) << 11;
            return InstEncodeData(ret, true);
        }
    }
    else if (instType == InstType::I) {
        unsigned opCode = InstLookUp::translateToOpCode(op);
        if (op == "lui") {
            unsigned rt, c;
            rt = getReg(nextString(inst));
            c = getC(nextString(inst));
            unsigned ret = 0u;
            ret |= (opCode & 0x3Fu) << 26;
            ret |= (rt & 0x1Fu) << 16;
            ret |= (c & 0xFFFFu);
            return InstEncodeData(ret, true);
        }
        else if (op == "bgtz") {
            unsigned rs, c;
            rs = getReg(nextString(inst));
            c = getBranchC(nextString(inst));
            unsigned ret = 0u;
            ret |= (opCode & 0x3Fu) << 26;
            ret |= (rs & 0x1Fu) << 21;
            ret |= (c & 0xFFFFu);
            return InstEncodeData(ret, true);
        }
        else if (op == "beq" || op == "bne") {
            unsigned rs, rt, c;
            rs = getReg(nextString(inst));
            rt = getReg(nextString(inst));
            c = getBranchC(nextString(inst));
            unsigned ret = 0u;
            ret |= (opCode & 0x3Fu) << 26;
            ret |= (rs & 0x1Fu) << 21;
            ret |= (rt & 0x1Fu) << 16;
            ret |= (c & 0xFFFFu);
            return InstEncodeData(ret, true);
        }
        else if (op == "lw" ||
                 op == "lh" ||
                 op == "lhu" ||
                 op == "lb" ||
                 op == "lbu" ||
                 op == "sw" ||
                 op == "sh" ||
                 op == "sb") {
            unsigned rt, rs;
            unsigned c;
            rt = getReg(nextString(inst));
            c = getC(nextString(inst));
            rs = getReg(nextString(inst));
            unsigned ret = 0u;
            ret |= (opCode & 0x3Fu) << 26;
            ret |= (rs & 0x1Fu) << 21;
            ret |= (rt & 0x1Fu) << 16;
            ret |= (c & 0xFFFFu);
            return InstEncodeData(ret, true);
        }
        else {
            unsigned rs, rt, c;
            rt = getReg(nextString(inst));
            rs = getReg(nextString(inst));
            c = getC(nextString(inst));
            unsigned ret = 0u;
            ret |= (opCode & 0x3Fu) << 26;
            ret |= (rs & 0x1Fu) << 21;
            ret |= (rt & 0x1Fu) << 16;
            ret |= (c & 0xFFFFu);
            return InstEncodeData(ret, true);
        }
    }
    else if (instType == InstType::J) {
        unsigned opCode = InstLookUp::translateToOpCode(op);
        unsigned c = static_cast<unsigned>(labelTable[nextString(inst)]);
        unsigned ret = 0u;
        ret |= (opCode & 0x3Fu) << 26;
        ret |= (c & 0x3FFFFFFu);
        return InstEncodeData(ret, true);
    }
    else if (instType == InstType::S) {
        return InstEncodeData(0xFFFFFFFFu, true);
    }
    else {
        fprintf(stderr, "Line %d: Unrecognized instruction %s\n", line, op);
        return InstEncodeData(0xFFFFFFFFu, false);
    }
}

unsigned InstEncoder::getReg(const std::string &src) {
    std::string tmp = toLowerString(src);
    if (tmp[0] == '$') {
        tmp = tmp.substr(1);
    }
    unsigned regNum;
    if (isdigit(tmp[0])) {
        sscanf(tmp.c_str(), "%u", &regNum);
    }
    else {
        regNum = InstLookUp::translateToReg(tmp);
    }
    return regNum;
}

unsigned InstEncoder::getC(const std::string &src) {
    unsigned ret;
    if (src.find("0x") != std::string::npos) {
        sscanf(src.c_str(), "%x", &ret);
    }
    else {
        sscanf(src.c_str(), "%d", &ret);
    }
    return ret;
}

unsigned InstEncoder::getBranchC(const std::string &src) {
    unsigned ret;
    if (sscanf(src.c_str(), "%x", &ret) == 1) {
        return ret;
    }
    else {
        if (!labelTable.count(src)) {
            fprintf(stderr, "Line %d: Undefined label: %s\n", line, src.c_str());
            return 0u;
        }
        int targetAddr = labelTable[src];
        int offset = (targetAddr - pc - 4) >> 2;
        return toUnsigned(offset);
    }
}

InstType InstEncoder::getInstType(const std::string& inst) {
    if (inst == "halt") {
        return InstType::S;
    }
    else if (inst == "j" || inst == "jal") {
        return InstType::J;
    }
    else if (inst == "add" ||
             inst == "addu" ||
             inst == "sub" ||
             inst == "and" ||
             inst == "or" ||
             inst == "xor" ||
             inst == "nor" ||
             inst == "nand" ||
             inst == "slt" ||
             inst == "sll" ||
             inst == "srl" ||
             inst == "sra" ||
             inst == "jr") {
        return InstType::R;
    }
    else if (inst == "addi" ||
             inst == "addiu" ||
             inst == "lw" ||
             inst == "lh" ||
             inst == "lhu" ||
             inst == "lb" ||
             inst == "lbu" ||
             inst == "sw" ||
             inst == "sh" ||
             inst == "sb" ||
             inst == "lui" ||
             inst == "andi" ||
             inst == "ori" ||
             inst == "nori" ||
             inst == "slti" ||
             inst == "beq" ||
             inst == "bne" ||
             inst == "bgtz") {
        return InstType::I;
    }
    else {
        return InstType::Undef;
    }
}

bool InstEncoder::hasLabel(const std::string& src) {
    return src.find(":", 0) != std::string::npos;
}

bool InstEncoder::isComment(const std::string& src) {
    for (unsigned i = 0; i < src.length(); ++i) {
        if (src[i] != ' ' && src[i] != '\t') {
            if (src[i] == '#') {
                return true;
            }
        }
    }
    return false;
}

std::string InstEncoder::nextString(std::string &src) {
    char inputBuffer[2048];
    std::string stringBuffer;
    sscanf(src.c_str(), "%s", inputBuffer);
    stringBuffer = inputBuffer;
    unsigned deleteIdx = 0;
    while (deleteIdx < src.length() && src[deleteIdx] == ' ') {
        ++deleteIdx;
    }
    while (deleteIdx < src.length() && src[deleteIdx] != ' ') {
        ++deleteIdx;
    }
    if (deleteIdx >= src.length()) {
        src = "";
    }
    else {
        src = src.substr(deleteIdx);
    }
    return stringBuffer;
}

std::string InstEncoder::opToLower(const std::string &src) {
    std::string ret = src;
    for (unsigned i = 0; i < ret.length(); ++i) {
        if (ret[i] == ' ') {
            break;
        }
        ret[i] = static_cast<char>(tolower(ret[i]));
    }
    return ret;
}

std::string InstEncoder::toLowerString(const std::string &src) {
    std::string ret = src;
    for (unsigned i = 0; i < ret.length(); ++i) {
        ret[i] = static_cast<char>(tolower(ret[i]));
    }
    return ret;
}

std::string InstEncoder::processInputString(const std::string &inst) {
    std::string val = inst;
    std::string ret = "";
    val = trimLeadingWhiteSpace(val);
    unsigned idx = 0;
    for ( ; idx < val.length(); ++idx) {
        ret += val[idx];
        if (val[idx] == ' ') {
            break;
        }
    }
    for ( ; idx < val.length(); ++idx) {
        if (val[idx] == ',') {
            ret += " ";
            continue;
        }
        if (val[idx] == '$') {
            ret += " ";
        }
        if (val[idx] == ' ') {
            continue;
        }
        ret += val[idx];
    }
    return ret;
}

std::string InstEncoder::trimWhiteSpace(const std::string &src) {
    std::string ret = "";
    for (unsigned i = 0; i < src.length(); ++i) {
        if (src[i] == ' ') {
            continue;
        }
        if (src[i] == '#') {
            break;
        }
        ret += src[i];
    }
    return ret;
}

std::string InstEncoder::trimLeadingWhiteSpace(const std::string &src) {
    unsigned idx = 0;
    while (idx < src.length() && src[idx] == ' ') {
        ++idx;
    }
    if (idx >= src.length()) {
        return "";
    }
    else {
        return src.substr(idx);
    }
}

} /* namespace lb */
