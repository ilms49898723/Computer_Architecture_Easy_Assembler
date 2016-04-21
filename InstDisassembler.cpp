/*
 * InstDisassembler.cpp
 *
 *  Created on: 2016/04/12
 *      Author: LittleBird
 */

#include "InstDisassembler.h"

namespace lb {

InstDisassembler::InstDisassembler() {
    this->useLabel = true;
    this->useHex = false;
    this->labelCount = 0;
    this->maxPc = 0;
    this->len = 0;
    memset(inst, 0, sizeof(unsigned) * 1024);
    assembly.clear();
    labelTable.clear();
    for (int i = 0; i < 1024; ++i) {
        result[i] = "";
    }
}

InstDisassembler::~InstDisassembler() {

}

void InstDisassembler::init(const unsigned *inst, const unsigned &len, const unsigned& pc) {
    for (unsigned i = pc / 4, j = 0; j < len; ++i, ++j) {
        this->inst[i] = inst[j];
    }
    this->len = pc / 4 + len;
}

void InstDisassembler::start() {
    for (int i = 0; i < 1024; ++i) {
        result[i] = "";
    }
    for (unsigned i = 0; i < len; ++i) {
        InstDataStr ret = InstDecoder::decodeInstStr(inst[i], useHex);
        if (ret.getType() == InstType::Undef) {
            fprintf(stderr, "Warning: Instruction #%d: Undefined Instruction was found. Replaced by nop\n", i + 1);
            ret = InstDecoder::decodeInstStr(0u, useHex);
        }
        assembly.push_back(ret.toString());
    }
    if (useLabel) {
        for (unsigned i = 0; i < assembly.size(); ++i) {
            std::string &current = assembly[i];
            if (current.find("beq ") != std::string::npos ||
                current.find("bne ") != std::string::npos) {
                char op[1024], rs[1024], rt[1024], c[1024];
                sscanf(current.c_str(), "%s%s%s%s", op, rs, rt, c);
                int offset;
                if (useHex) {
                    sscanf(c, "%x", &offset);
                }
                else {
                    sscanf(c, "%d", &offset);
                }
                offset = toSigned(static_cast<unsigned>(offset), 16);
                offset = (offset * 4 + 4) >> 2;
                maxPc = std::max(i + offset, maxPc);
                if (!labelTable.count(i + offset)) {
                    char temp[1024];
                    sprintf(temp, "%d", labelCount);
                    ++labelCount;
                    std::string newLabel = "L" + std::string(temp);
                    labelTable.insert(std::make_pair(i + offset, newLabel));
                }
                current = std::string(op) + " " + std::string(rs) + " " + std::string(rt) + " " +
                          labelTable[i + offset];
            }
            else if (current.find("bgtz ") != std::string::npos) {
                char op[1024], rs[1024], c[1024];
                sscanf(current.c_str(), "%s%s%s", op, rs, c);
                int offset;
                if (useHex) {
                    sscanf(c, "%x", &offset);
                }
                else {
                    sscanf(c, "%d", &offset);
                }
                offset = toSigned(static_cast<unsigned>(offset), 16);
                offset = (offset * 4 + 4) >> 2;
                maxPc = std::max(i + offset, maxPc);
                if (!labelTable.count(i + offset)) {
                    char temp[1024];
                    sprintf(temp, "%d", labelCount);
                    ++labelCount;
                    std::string newLabel = "L" + std::string(temp);
                    labelTable.insert(std::make_pair(i + offset, newLabel));
                }
                current = std::string(op) + " " + std::string(rs) + " " + labelTable[i + offset];
            }
            else if (current.find("j ") != std::string::npos ||
                     current.find("jal ") != std::string::npos) {
                char op[1024], c[1024];
                sscanf(current.c_str(), "%s%s", op, c);
                int offset;
                if (useHex) {
                    sscanf(c, "%x", &offset);
                }
                else {
                    sscanf(c, "%d", &offset);
                }
                maxPc = std::max(i + offset, maxPc);
                if (!labelTable.count(i + offset)) {
                    char temp[1024];
                    sprintf(temp, "%d", labelCount);
                    ++labelCount;
                    std::string newLabel = "L" + std::string(temp);
                    labelTable.insert(std::make_pair(i + offset, newLabel));
                }
                current = std::string(op) + " " + labelTable[i + offset];
            }
        }
    }
    for (unsigned i = 0; i < assembly.size(); ++i) {
        if (useLabel) {
            if (labelTable.count(i)) {
                for (unsigned long long j = labelTable[i].length(); j < 4; ++j) {
                    result[i] += " ";
                }
                result[i] += labelTable[i] + ": ";
            }
            else {
                result[i] += "      ";
            }
        }
        if (assembly[i] == "sll $0, $0, 0") {
            result[i] += "nop";
        }
        else {
            result[i] += assembly[i];
        }
    }
    for (unsigned i = static_cast<unsigned>(assembly.size()); i <= maxPc; ++i) {
        if (useLabel) {
            if (labelTable.count(i)) {
                for (unsigned long long j = labelTable[i].length(); j < 4; ++j) {
                    result[i] += " ";
                }
                result[i] += labelTable[i] + ": ";
            }
            else {
                result[i] += "      ";
            }
        }
        result[i] += "nop";
    }
    len = std::max(maxPc + 1, static_cast<unsigned>(assembly.size()));
}

void InstDisassembler::setUseLabel(bool useLabel) {
    this->useLabel = useLabel;
}

void InstDisassembler::setUseHex(bool useHex) {
    this->useHex = useHex;
}

unsigned InstDisassembler::length() const {
    return len;
}

std::string InstDisassembler::getLine(const unsigned &line) const {
    if (line >= len) {
        return "";
    }
    return result[line];
}

}
