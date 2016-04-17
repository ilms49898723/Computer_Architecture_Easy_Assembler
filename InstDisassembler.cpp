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
    this->labelCount = 0;
    this->len = this->pc = 0;
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
        this->inst[i] = inst[i];
    }
    this->len = pc / 4 + len;
}

void InstDisassembler::start() {
    for (int i = 0; i < 1024; ++i) {
        result[i] = "";
    }
    for (unsigned i = 0; i < len; ++i) {
        InstDataStr ret = InstDecoder::decodeInstStr(inst[i]);
        if (ret.getType() == InstType::Undef) {
            fprintf(stderr, "Warning: Undefined Instruction was found. Replaced by nop\n");
            ret = InstDecoder::decodeInstStr(0u);
        }
        assembly.push_back(ret.toString());
    }
    if (useLabel) {
        for (unsigned i = 0; i < assembly.size(); ++i) {
            std::string &current = assembly[i];
            if (current.find("beq") != std::string::npos ||
                current.find("bne") != std::string::npos) {
                char op[1024], rs[1024], rt[1024], c[1024];
                sscanf(current.c_str(), "%s%s%s%s", op, rs, rt, c);
                int offset;
                sscanf(c, "%x", &offset);
                offset = toSigned(static_cast<unsigned>(offset), 16);
                offset = (offset * 4 + 4) >> 2;
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
            else if (current.find("bgtz") != std::string::npos) {
                char op[1024], rs[1024], c[1024];
                sscanf(current.c_str(), "%s%s%s", op, rs, c);
                int offset;
                sscanf(c, "%x", &offset);
                offset = toSigned(static_cast<unsigned>(offset), 16);
                offset = (offset * 4 + 4) >> 2;
                if (!labelTable.count(i + offset)) {
                    char temp[1024];
                    sprintf(temp, "%d", labelCount);
                    ++labelCount;
                    std::string newLabel = "L" + std::string(temp);
                    labelTable.insert(std::make_pair(i + offset, newLabel));
                }
                current = std::string(op) + " " + std::string(rs) + " " + labelTable[i + offset];
            }
            else if (current.find("j") != std::string::npos ||
                     current.find("jal") != std::string::npos) {
                char op[1024], c[1024];
                sscanf(current.c_str(), "%s%s", op, c);
                int offset;
                sscanf(c, "%x", &offset);
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
        result[i] += assembly[i];
    }
}

void InstDisassembler::setUseLabel(bool useLabel) {
    this->useLabel = useLabel;
}

std::string InstDisassembler::getLine(const unsigned &line) {
    if (line >= len) {
        return "";
    }
    return result[line];
}

}
