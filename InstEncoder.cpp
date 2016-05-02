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

void InstEncoder::setPc(const unsigned& pc) {
    this->pc = pc;
    this->preprocessPc = pc;
}

void InstEncoder::processLabel(std::string& src) {
    if (isEmptyOrCommentLine(src)) {
        return;
    }
    if (hasLabel(src)) {
        unsigned long long loc = src.find(":");
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

void InstEncoder::printErrorMessage(const std::string& msg, const int& idx) {
    std::string prefix = "At Line " + std::to_string(line) + ":" + std::to_string(elementsLocation[idx]) + ": ";
    fprintf(stderr, "%s%s\n", prefix.c_str(), originalInputString.c_str());
    for (int i = 0; i < static_cast<int>(prefix.length()); ++i) {
        fprintf(stderr, " ");
    }
    for (int i = 0; i < elementsLocation[idx]; ++i) {
        fprintf(stderr, " ");
    }
    fprintf(stderr, "^\n");
    fprintf(stderr, "    ");
    fprintf(stderr, "%s\n", msg.c_str());
}

int InstEncoder::splitInputString(const std::string& src) {
    int idx = 0;
    std::string temp = src;
    temp = removeComment(temp);
    temp = trimLeadingWhiteSpace(temp);
    elements[idx] = nextString(temp);
    elementsLocation[idx] = 0;
    ++idx;
    while (!temp.empty()) {
        if (idx >= 15) {
            return 0;
        }
        unsigned long long i = 0;
        std::string newElement;
        if (temp[0] == '(') {
            while (i < temp.length() && temp[i] != ')') {
                ++i;
            }
            if (i >= temp.length()) {
                unsigned long long loc = originalInputString.find("(");
                std::string prefix = "At Line " + std::to_string(line) + ":" + std::to_string(loc) + ": ";
                fprintf(stderr, "%s%s\n", prefix.c_str(), originalInputString.c_str());
                for (unsigned long long j = 0; j < prefix.length(); ++j) {
                    fprintf(stderr, " ");
                }
                for (unsigned long long j = 0; j < loc; ++j) {
                    fprintf(stderr, " ");
                }
                fprintf(stderr, "^\n");
                fprintf(stderr, "    Syntax Error: Mismatched parentheses\n");
                return 0;
            }
            newElement = temp.substr(1, i - 1);
        }
        else {
            while (i < temp.length() && temp[i] != ',' && temp[i] != '(' && temp[i] != ')') {
                ++i;
            }
            if (temp[i] == ')') {
                unsigned long long loc = originalInputString.find(")");
                std::string prefix = "At Line " + std::to_string(line) + ":" + std::to_string(loc) + ": ";
                fprintf(stderr, "%s%s\n", prefix.c_str(), originalInputString.c_str());
                for (unsigned long long j = 0; j < prefix.length(); ++j) {
                    fprintf(stderr, " ");
                }
                for (unsigned long long j = 0; j < loc; ++j) {
                    fprintf(stderr, " ");
                }
                fprintf(stderr, "^\n");
                fprintf(stderr, "    Syntax Error: Mismatched parentheses\n");
                return 0;
            }
            newElement = temp.substr(0, i);
        }
        elements[idx] = trimWhiteSpace(newElement);
        unsigned long long startIdx = elementsLocation[idx - 1] + elements[idx - 1].length();
        elementsLocation[idx] = static_cast<int>(src.find(newElement, startIdx));
        while (src[elementsLocation[idx]] == ' ' || src[elementsLocation[idx]] == '\t') {
            ++elementsLocation[idx];
            if (elementsLocation[idx] >= static_cast<int>(src.length())) {
                break;
            }
        }
        ++idx;
        if (temp[i] != '(') {
            if (i + 1 < temp.length()) {
                temp = temp.substr(i + 1);
            }
            else {
                temp = "";
            }
        }
        else {
            if (i < temp.length()) {
                temp = temp.substr(i);
            }
            else {
                temp = "";
            }
        }
    }
    return idx;
}

InstEncodeData InstEncoder::analyzeString(const std::string& inst) {
    originalInputString = inst;
    elementsLength = splitInputString(inst);
    if (elementsLength <= 0) {
        valid = false;
        return InstEncodeData();
    }
    elements[0] = toLowerString(elements[0]);
    // nop
    if (getElements(0) == "nop") {
        if (!checkElementsCount(1)) {
            valid = false;
            return InstEncodeData();
        }
        return InstEncodeData(0u, true);
    }
    // normal case
    // for instruction operator
    std::string op = getElements(0);
    InstType instType = getInstType(op);
    if (instType == InstType::R) {
        unsigned funct = InstLookUp::translateToFunct(op);
        if (op == "jr") {
            if (!checkElementsCount(2)) {
                valid = false;
                return InstEncodeData();
            }
            unsigned rs;
            rs = getReg(getElements(1), 1);
            unsigned ret = 0u;
            ret |= funct & 0x3Fu;
            ret |= (rs & 0x1Fu) << 21;
            return InstEncodeData(ret, true);
        }
        else if (op == "sll" ||
                 op == "srl" ||
                 op == "sra") {
            if (!checkElementsCount(4)) {
                valid = false;
                return InstEncodeData();
            }
            unsigned rt, rd, c;
            rd = getReg(getElements(1), 1);
            rt = getReg(getElements(2), 2);
            c = getC(getElements(3), 3);
            unsigned ret = 0u;
            ret |= funct & 0x3Fu;
            ret |= (rt & 0x1Fu) << 16;
            ret |= (rd & 0x1Fu) << 11;
            ret |= (c & 0x1Fu) << 6;
            return InstEncodeData(ret, true);
        }
        else {
            if (!checkElementsCount(4)) {
                valid = false;
                return InstEncodeData();
            }
            unsigned rs, rt, rd;
            rd = getReg(getElements(1), 1);
            rs = getReg(getElements(2), 2);
            rt = getReg(getElements(3), 3);
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
            if (!checkElementsCount(3)) {
                valid = false;
                return InstEncodeData();
            }
            unsigned rt, c;
            rt = getReg(getElements(1), 1);
            c = getC(getElements(2), 2);
            unsigned ret = 0u;
            ret |= (opCode & 0x3Fu) << 26;
            ret |= (rt & 0x1Fu) << 16;
            ret |= (c & 0xFFFFu);
            return InstEncodeData(ret, true);
        }
        else if (op == "bgtz") {
            if (!checkElementsCount(3)) {
                valid = false;
                return InstEncodeData();
            }
            unsigned rs, c;
            rs = getReg(getElements(1), 1);
            c = getBranchC(getElements(2), 2);
            unsigned ret = 0u;
            ret |= (opCode & 0x3Fu) << 26;
            ret |= (rs & 0x1Fu) << 21;
            ret |= (c & 0xFFFFu);
            return InstEncodeData(ret, true);
        }
        else if (op == "beq" || op == "bne") {
            if (!checkElementsCount(4)) {
                valid = false;
                return InstEncodeData();
            }
            unsigned rs, rt, c;
            rs = getReg(getElements(1), 1);
            rt = getReg(getElements(2), 2);
            c = getBranchC(getElements(3), 3);
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
            if (!checkElementsCount(4)) {
                valid = false;
                return InstEncodeData();
            }
            unsigned rt, rs;
            unsigned c;
            rt = getReg(getElements(1), 1);
            c = getC(getElements(2), 2);
            rs = getReg(getElements(3), 3);
            unsigned ret = 0u;
            ret |= (opCode & 0x3Fu) << 26;
            ret |= (rs & 0x1Fu) << 21;
            ret |= (rt & 0x1Fu) << 16;
            ret |= (c & 0xFFFFu);
            return InstEncodeData(ret, true);
        }
        else {
            if (!checkElementsCount(4)) {
                valid = false;
                return InstEncodeData();
            }
            unsigned rs, rt, c;
            rt = getReg(getElements(1), 1);
            rs = getReg(getElements(2), 2);
            c = getC(getElements(3), 3);
            unsigned ret = 0u;
            ret |= (opCode & 0x3Fu) << 26;
            ret |= (rs & 0x1Fu) << 21;
            ret |= (rt & 0x1Fu) << 16;
            ret |= (c & 0xFFFFu);
            return InstEncodeData(ret, true);
        }
    }
    else if (instType == InstType::J) {
        if (!checkElementsCount(2)) {
            valid = false;
            return InstEncodeData();
        }
        unsigned opCode = InstLookUp::translateToOpCode(op);
        unsigned c;
        unsigned ret = 0u;
        std::string next = getElements(1);
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
            c = static_cast<unsigned>(labelTable[next]) >> 2;
        }
        else {
            char errorMessage[1100];
            sprintf(errorMessage, "Syntax Error: %s: Undefined label or Not a number\n", next.c_str());
            printErrorMessage(errorMessage, 1);
            valid = false;
            return InstEncodeData();
        }
        ret |= (opCode & 0x3Fu) << 26;
        ret |= (c & 0x3FFFFFFu);
        return InstEncodeData(ret, true);
    }
    else if (instType == InstType::S) {
        if (!checkElementsCount(1)) {
            valid = false;
            return InstEncodeData();
        }
        return InstEncodeData(0xFFFFFFFFu, true);
    }
    else {
        char errorMessage[1100];
        sprintf(errorMessage, "Syntax Error: %s: Undefined instruction", op.c_str());
        printErrorMessage(errorMessage, 0);
        valid = false;
        return InstEncodeData(0xFFFFFFFFu, false);
    }
}

unsigned InstEncoder::getReg(const std::string& src, const int& cnt) {
    if (!valid) {
        return 0xFFFFFFFFu;
    }
    if (isEmptyOrCommentLine(src)) {
        char errorMessage[1100];
        sprintf(errorMessage, "Syntax Error: Missing Arguments");
        printErrorMessage(errorMessage, cnt);
        valid = false;
        return 0xFFFFFFFFu;
    }
    std::string tmp = toLowerString(src);
    if (tmp[0] != '$') {
        char errorMessage[1100];
        sprintf(errorMessage, "Syntax Error: Register expected, %s provided", src.c_str());
        printErrorMessage(errorMessage, cnt);
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
            char errorMessage[1100];
            sprintf(errorMessage, "Syntax Error: %s: Undefined register", src.c_str());
            printErrorMessage(errorMessage, cnt);
            valid = false;
        }
    }
    return regNum;
}

unsigned InstEncoder::getC(const std::string& src, const int& cnt) {
    if (!valid) {
        return 0xFFFFFFFFu;
    }
    if (isEmptyOrCommentLine(src)) {
        char errorMessage[1100];
        sprintf(errorMessage, "Syntax Error: Missing Arguments");
        printErrorMessage(errorMessage, cnt);
        valid = false;
        return 0xFFFFFFFFu;
    }
    unsigned ret;
    if (!isNumber(src)) {
        char errorMessage[1100];
        sprintf(errorMessage, "Syntax Error: %s: Not a number", src.c_str());
        printErrorMessage(errorMessage, cnt);
        valid = false;
        return 0xFFFFFFFFu;
    }
    if (src.find("0x") != std::string::npos) {
        sscanf(src.c_str(), "%x", &ret);
    }
    else {
        sscanf(src.c_str(), "%d", &ret);
    }
    return ret;
}

unsigned InstEncoder::getBranchC(const std::string& src, const int& cnt) {
    if (!valid) {
        return 0xFFFFFFFFu;
    }
    if (isEmptyOrCommentLine(src)) {
        char errorMessage[1100];
        sprintf(errorMessage, "Syntax Error: Missing Arguments");
        printErrorMessage(errorMessage, cnt);
        valid = false;
        return 0xFFFFFFFFu;
    }
    unsigned ret;
    if (isNumber(src)) {
        if (src.find("0x") != std::string::npos) {
            sscanf(src.c_str(), "%x", &ret);
            return ret;
        }
        else {
            sscanf(src.c_str(), "%d", &ret);
            return ret;
        }
    }
    else {
        if (!labelTable.count(src)) {
            char errorMessage[1100];
            sprintf(errorMessage, "Syntax Error: %s: Undefined label or Not a number", src.c_str());
            printErrorMessage(errorMessage, cnt);
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

bool InstEncoder::isEmptyOrCommentLine(const std::string& src) {
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

bool InstEncoder::isNumber(const std::string& src) {
    if (src.empty()) {
        return false;
    }
    bool isHex = false;
    unsigned i = 0;
    if (src[i] == '-') {
        i += 1;
    }
    if (i + 1 < src.length() && src[i] == '0' && tolower(src[i + 1]) == 'x') {
        i += 2;
        isHex = true;
    }
    for (; i < src.length(); ++i) {
        if (isHex && tolower(src[i]) >= 'a' && tolower(src[i]) <= 'f') {
            continue;
        }
        if (!isdigit(src[i])) {
            return false;
        }
    }
    return true;
}

bool InstEncoder::checkElementsCount(const int& cnt) {
    if (elementsLength != cnt) {
        printErrorMessage("Invalid Instructions, too many/few arguments, or mistyped", 0);
    }
    return elementsLength == cnt;
}

std::string InstEncoder::getElements(const int& target) {
    if (target < 0 || target >= elementsLength) {
        return "";
    }
    return elements[target];
}

std::string InstEncoder::nextString(std::string& src) {
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

std::string InstEncoder::toLowerString(const std::string& src) {
    std::string ret = src;
    for (unsigned i = 0; i < ret.length(); ++i) {
        ret[i] = static_cast<char>(tolower(ret[i]));
    }
    return ret;
}

std::string InstEncoder::trimWhiteSpace(const std::string& src) {
    std::string ret = "";
    for (unsigned i = 0; i < src.length(); ++i) {
        if (src[i] == ' ' || src[i] == '\t') {
            continue;
        }
        if (src[i] == '#') {
            break;
        }
        ret += src[i];
    }
    return ret;
}

std::string InstEncoder::trimLeadingWhiteSpace(const std::string& src) {
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

std::string InstEncoder::removeComment(const std::string& src) {
    unsigned long long loc = src.find("#");
    if (loc == std::string::npos) {
        return src;
    }
    else {
        return src.substr(0, loc);
    }
}

} /* namespace lb */
