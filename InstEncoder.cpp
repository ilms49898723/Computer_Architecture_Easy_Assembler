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
    valid = true;
    pc = 0u;
    preprocessPc = 0u;
    line = 1;
}

void InstEncoder::setPc(const unsigned &pc) {
    this->pc = pc;
    this->preprocessPc = pc;
}

void InstEncoder::processLabel(std::string &src) {
    if (isEmptyOrCommentLine(src)) {
        return;
    }
    if (hasLabel(src)) {
        unsigned long long loc = src.find(':');
        std::string label = trimWhiteSpace(src.substr(0, loc));
        src = src.substr(loc + 1);
        src = trimLeadingWhiteSpace(src);
        labelTable.insert(make_pair(label, preprocessPc));
        preprocessPc += 4;
        return;
    }
    else {
        src = trimLeadingWhiteSpace(src);
        preprocessPc += 4;
        return;
    }
}

InstEncodeData InstEncoder::encodeInst(const std::string& inst) {
    if (!valid) {
        return InstEncodeData();
    }
    if (isEmptyOrCommentLine(inst)) {
        ++line;
        return InstEncodeData(0u, false);
    }
    std::string val = inst;
    InstEncodeData data = analyzeString(val);
    pc += 4;
    ++line;
    if (valid) {
        return data;
    }
    else {
        return InstEncodeData();
    }
}

InstEncodeData InstEncoder::analyzeString(std::string inst) {
    originalInputString = inst;
    inst = removeComment(inst);
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
        unsigned c;
        unsigned ret = 0u;
        std::string next = nextString(inst);
        if (isNumber(next)) {
            int temp;
            if (next.length() > 2 && next[0] == '0' && tolower(next[1]) == 'x') {
                sscanf(next.c_str() + 2, "%x", &temp);
            }
            else {
                sscanf(next.c_str(), "%d", &temp);
            }
            c = static_cast<unsigned>(temp);
        }
        else if (labelTable.count(next)) {
            c = static_cast<unsigned>(labelTable[next]);
        }
        else {
            std::string prefixString = "Line " + std::to_string(line) + ": " + originalInputString;
            fprintf(stderr, "%s\n", prefixString.c_str());
            unsigned long long spaceCnt = prefixString.find(next);
            for (unsigned long long i = 0; i < spaceCnt; ++i) {
                fprintf(stderr, " ");
            }
            fprintf(stderr, "^\n");
            fprintf(stderr, "    Syntax Error: %s: Invalid arguments for jr\n", next.c_str());
            valid = false;
            return InstEncodeData();
        }
        ret |= (opCode & 0x3Fu) << 26;
        ret |= (c & 0x3FFFFFFu);
        return InstEncodeData(ret, true);
    }
    else if (instType == InstType::S) {
        return InstEncodeData(0xFFFFFFFFu, true);
    }
    else {
        std::string prefixString = "Line " + std::to_string(line) + ": " + originalInputString;
        fprintf(stderr, "%s\n", prefixString.c_str());
        for (unsigned long long i = 0; i < 4; ++i) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "^\n");
        fprintf(stderr, "    Syntax Error: %s: Undefined instruction\n", op.c_str());
        valid = false;
        return InstEncodeData(0xFFFFFFFFu, false);
    }
}

unsigned InstEncoder::getReg(const std::string &src) {
    if (!valid) {
        return 0xFFFFFFFFu;
    }
    if (isEmptyOrCommentLine(src)) {
        std::string prefixString = "Line " + std::to_string(line) + ": " + originalInputString;
        fprintf(stderr, "%s\n", prefixString.c_str());
        for (unsigned long long i = 0; i < prefixString.length() + 1; ++i) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "^\n");
        fprintf(stderr, "    Syntax Error: Missing Arguments\n");
        valid = false;
        return 0xFFFFFFFFu;
    }
    std::string tmp = toLowerString(src);
    if (tmp[0] != '$') {
        std::string prefixString = "Line " + std::to_string(line) + ": " + originalInputString;
        fprintf(stderr, "%s\n", prefixString.c_str());
        unsigned long long spaceCnt = prefixString.find(src);
        for (unsigned long long i = 0; i < spaceCnt; ++i) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "^\n");
        fprintf(stderr, "    Syntax Error: register expected, %s provided\n", src.c_str());
        valid = false;
        return 0xFFFFFFFFu;
    }
    tmp = tmp.substr(1);
    unsigned regNum;
    if (isdigit(tmp[0])) {
        sscanf(tmp.c_str(), "%u", &regNum);
    }
    else {
        regNum = InstLookUp::translateToReg(tmp);
        if (regNum == 0xFFFFFFFFu) {
            std::string prefixString = "Line " + std::to_string(line) + ": " + originalInputString;
            fprintf(stderr, "%s\n", prefixString.c_str());
            unsigned long long spaceCnt = prefixString.find(src);
            for (unsigned long long i = 0; i < spaceCnt; ++i) {
                fprintf(stderr, " ");
            }
            fprintf(stderr, "^\n");
            fprintf(stderr, "    Syntax Error: %s: Undefined register\n", src.c_str());
            valid = false;
        }
    }
    return regNum;
}

unsigned InstEncoder::getC(const std::string &src) {
    if (!valid) {
        return 0xFFFFFFFFu;
    }
    if (isEmptyOrCommentLine(src)) {
        std::string prefixString = "Line " + std::to_string(line) + ": " + originalInputString;
        fprintf(stderr, "%s\n", prefixString.c_str());
        for (unsigned long long i = 0; i < prefixString.length() + 1; ++i) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "^\n");
        fprintf(stderr, "    Syntax Error: Missing Arguments\n");
        valid = false;
        return 0xFFFFFFFFu;
    }
    unsigned ret;
    if (!isNumber(src)) {
        std::string prefixString = "Line " + std::to_string(line) + ": " + originalInputString;
        fprintf(stderr, "%s\n", prefixString.c_str());
        unsigned long long spaceCnt = prefixString.find(src);
        for (unsigned long long i = 0; i < spaceCnt; ++i) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "^\n");
        fprintf(stderr, "    Syntax Error: %s: Not a number\n", src.c_str());
        valid = false;
        return 0xFFFFFFFFu;
    }
    if (src.find("0x") != std::string::npos || src.find("0X") != std::string::npos) {
        sscanf(src.c_str(), "%x", &ret);
    }
    else {
        sscanf(src.c_str(), "%d", &ret);
    }
    return ret;
}

unsigned InstEncoder::getBranchC(const std::string &src) {
    if (!valid) {
        return 0xFFFFFFFFu;
    }
    if (isEmptyOrCommentLine(src)) {
        std::string prefixString = "Line " + std::to_string(line) + ": " + originalInputString;
        fprintf(stderr, "%s\n", prefixString.c_str());
        for (unsigned long long i = 0; i < prefixString.length() + 1; ++i) {
            fprintf(stderr, " ");
        }
        fprintf(stderr, "^\n");
        fprintf(stderr, "    Syntax Error: Missing Arguments\n");
        valid = false;
        return 0xFFFFFFFFu;
    }
    unsigned ret;
    if (sscanf(src.c_str(), "%x", &ret) == 1) {
        return ret;
    }
    else {
        if (!labelTable.count(src)) {
            std::string prefixString = "Line " + std::to_string(line) + ": " + originalInputString;
            fprintf(stderr, "%s\n", prefixString.c_str());
            unsigned long long spaceCnt = prefixString.find(src);
            for (unsigned long long i = 0; i < spaceCnt; ++i) {
                fprintf(stderr, " ");
            }
            fprintf(stderr, "^\n");
            fprintf(stderr, "    Syntax Error: %s: Undefined label\n", src.c_str());
            valid = false;
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
    return src.find(":") != std::string::npos;
}

bool InstEncoder::isEmptyOrCommentLine(const std::string &src) {
    // check for empty line
    bool isEmpty = true;
    for (unsigned i = 0; i < src.length(); ++i) {
        if (src[i] != ' ' && src[i] != '\t') {
            isEmpty = false;
        }
    }
    if (isEmpty) {
        return true;
    }
    // check for comment
    for (unsigned i = 0; i < src.length(); ++i) {
        if (src[i] != ' ' && src[i] != '\t' && src[i] != '#') {
            return false;
        }
        if (src[i] == '#') {
            return true;
        }
    }
    return false;
}

bool InstEncoder::isNumber(const std::string &src) {
    bool isHex = false;
    unsigned i = 0;
    if (src.length() > 2 && src[0] == '0' && tolower(src[1]) == 'x') {
        i = 2;
        isHex = true;
    }
    for ( ; i < src.length(); ++i) {
        if (isHex && tolower(src[i]) >= 'a' && tolower(src[i]) <= 'f') {
            continue;
        }
        if (!isdigit(src[i])) {
            return false;
        }
    }
    return true;
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
        if (val[idx] == ',' || val[idx] == '(' || val[idx] == ')') {
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

std::string InstEncoder::removeComment(const std::string &src) {
    unsigned long long loc = src.find("#");
    if (loc == std::string::npos) {
        return src;
    }
    else {
        return src.substr(0, loc);
    }
}

} /* namespace lb */
