/*
 * InstEncoder.cpp
 *
 *  Created on: 2016/04/12
 *      Author: LittleBird
 */

#include "InstEncoder.h"

namespace lb {

struct InstEncoder::InstDataEncode {
    std::string opCode;
    std::string rs, rt, rd;
    std::string c;
    std::string funct;
    bool isValid;
    InstDataEncode() {
        opCode = "";
        rs = "";
        rt = "";
        rd = "";
        c = "";
        funct = "";
        isValid = false;
    }
};

InstEncoder::InstEncoder() {
    init();
}

InstEncoder::~InstEncoder() {
}

void InstEncoder::init() {
    labelTable.clear();
    pc = line = 0;
    alive = false;
}

unsigned InstEncoder::encodeInst(const std::string& inst) {
    std::string val;
    if (inst.back() == '\n') {
        val = inst.substr(0, inst.length() - 1);
    }
    else {
        val = inst;
    }
    unsigned data = analyzeString(val);
    if (!isComment(val)) {
        // pc offset if this inst is valid, not comment
        pc += 4;
        // line offset
        ++line;
    }
    return data;
}

unsigned InstEncoder::analyzeString(std::string inst) {
    // if this line is a comment
    if (isComment(inst)) {
        alive = false;
        return 0xFFFFFFFFu;
    }
    // check whether this line contains Label like "ELSE: "
    if (hasLabel(inst)) {
        for (unsigned i = 0; i < inst.length(); ++i) {
            if (inst[i] == ':') {
                std::string label = trimWhiteSpace(inst.substr(0, i + 1));
                printf("label = \"%s\"\n", label.c_str());
                labelTable.insert(make_pair(label, pc));
                inst = inst.substr(i + 1);
            }
        }
    }
    inst = processInputString(inst);
    // nop
    if (toLowerString(inst) == "nop") {
        return 0u;
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
            return ret;
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
            return ret;
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
            return ret;
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
            return ret;
        }
        else if (op == "bgtz") {
            unsigned rs, c;
            rs = getReg(nextString(inst));
            c = getBranchC(nextString(inst));
            unsigned ret = 0u;
            ret |= (opCode & 0x3Fu) << 26;
            ret |= (rs & 0x1Fu) << 21;
            ret |= (c & 0xFFFFu);
            return ret;
        }
        else if (op == "beq" || op == "bne") {
            unsigned rs, c;
            rs = getReg(nextString(inst));
            c = getBranchC(nextString(inst));
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
            return ret;
        }
    }
    else if (instType == InstType::J) {
        unsigned opCode = InstLookUp::translateToOpCode(op);
        unsigned c = static_cast<unsigned>(labelTable[nextString(inst)]);
        unsigned ret = 0u;
        ret |= (opCode & 0x3Fu) << 26;
        ret |= (c & 0x3FFFFFFu);
        return ret;
    }
    else if (instType == InstType::S) {
        unsigned opCode = InstLookUp::translateToOpCode(op);
        unsigned ret = 0u;
        ret |= (opCode & 0x3Fu) << 26;
        return ret;
    }
    else {
        fprintf(stderr, "Line %d: Unrecognized instruction %s\n", line, instType);
        alive = false;
        return 0xFFFFFFFFu;
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

unsigned InstEncoder::getReg(const std::string &src) {
    std::string tmp = toLowerString(src);
    if (tmp[0] == '$') {
        tmp = tmp.substr(1);
    }
    int regNum;
    if (isdigit(tmp[0])) {
        sscanf(tmp.c_str(), "%d", &regNum);
    }
    else {
        regNum = InstLookUp::translateToReg(tmp);
    }
    return regNum;
}

unsigned InstEncoder::getC(const std::string &src) {
    unsigned ret;
    if (src.find("0x") != std::string::npos) {
        std::string val = src.substr(2);
        sscanf(val.c_str(), "%x", &ret);
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
        int targetAddr = labelTable[src];
        int offset = pc - targetAddr;
        return offset;
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

std::string InstEncoder::processInputString(const std::string &inst) {
    std::string ret = "";
    unsigned idx = 0;
    for (idx = 0; idx < inst.length(); ++idx) {
        if (inst[idx] != ' ') {
            break;
        }
    }
    for ( ; idx < inst.length(); ++idx) {
        ret += inst[idx];
        if (inst[idx] == ' ') {
            break;
        }
    }
    for ( ; idx < inst.length(); ++idx) {
        if (inst[idx] == '$') {
            ret += " ";
        }
        if (inst[idx] == ',') {
            ret += " ";
            continue;
        }
        if (inst[idx] == ' ') {
            continue;
        }
        ret += inst[idx];
    }
    return ret;
}

std::string InstEncoder::toLowerString(const std::string& src) {
    std::string ret = src;
    for (unsigned i = 0; i < ret.length(); ++i) {
        ret[i] = tolower(ret[i]);
    }
    return ret;
}

std::string InstEncoder::nextString(std::string &src) {
    char inputBuffer[2048];
    std::string stringBuffer;
    sscanf(src.c_str(), "%s", inputBuffer);
    stringBuffer = toLowerString(inputBuffer);
    unsigned leadingWhiteSpace = 0;
    while (leadingWhiteSpace < src.length() && src[leadingWhiteSpace] == ' ') {
        ++leadingWhiteSpace;
    }
    if (leadingWhiteSpace >= src.length()) {
        src = "";
    }
    else {
        src = src.substr(leadingWhiteSpace + stringBuffer.length());
    }
    return stringBuffer;
}

std::string InstEncoder::trimWhiteSpace(const std::string &src) {
    std::string ret = "";
    for (unsigned i = 0; i < src.length(); ++i) {
        if (src[i] == ' ' || src[i] == ':') {
            continue;
        }
        ret += src[i];
    }
    return ret;
}

} /* namespace lb */
