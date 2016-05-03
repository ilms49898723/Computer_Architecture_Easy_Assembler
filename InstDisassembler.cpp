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
    memset(inst, 0, sizeof(unsigned) * MAXN);
    assembly.clear();
    labelTable.clear();
    for (int i = 0; i < MAXN; ++i) {
        result[i] = "";
    }
}

InstDisassembler::~InstDisassembler() {

}

void InstDisassembler::init(const unsigned* inst, const unsigned& len, const unsigned& pc) {
    if (pc / 4 + len > MAXN) {
        fprintf(stderr, "Error: Disassembler: iimage is too large(max 4096 instructions)\n");
        exit(EXIT_FAILURE);
    }
    for (unsigned i = pc / 4, j = 0; j < len; ++i, ++j) {
        this->inst[i] = inst[j];
    }
    this->len = pc / 4 + len;
}

void InstDisassembler::start() {
    for (int i = 0; i < MAXN; ++i) {
        result[i] = "";
    }
    for (unsigned i = 0; i < len; ++i) {
        InstDataStr ret = InstDecoder::decodeInstStr(inst[i], useHex);
        if (ret.getType() == InstType::Undef) {
            fprintf(stderr, "Warning: Instruction #%d: Undefined Instruction 0x%08X was found.\n         Replaced by nop.\n", i + 1, inst[i]);
            ret = InstDecoder::decodeInstStr(0u, useHex);
        }
        assembly.push_back(ret.toString());
    }
    if (useLabel) {
        for (unsigned i = 0; i < assembly.size(); ++i) {
            std::string& current = assembly[i];
            if (current.find("beq ") != std::string::npos ||
                current.find("bne ") != std::string::npos) {
                char op[1024], rs[1024], rt[1024], c[1024];
                sscanf(current.c_str(), "%s%s%s%s", op, rs, rt, c);
                int targetAddr;
                if (useHex) {
                    sscanf(c, "%x", &targetAddr);
                }
                else {
                    sscanf(c, "%d", &targetAddr);
                }
                targetAddr = toSigned(static_cast<unsigned>(targetAddr), 16);
                targetAddr = (targetAddr * 4 + 4) >> 2;
                targetAddr += i;
                std::string targetAddrStr;
                if (useHex) {
                    targetAddrStr = toHexString(targetAddr);
                }
                else {
                    targetAddrStr = toDecString(targetAddr);
                }
                if (targetAddr + 1 > MAXN || targetAddr < 0) {
                    fprintf(stderr, "Warning: Instruction #%d: %s\n", i + 1, current.c_str());
                    fprintf(stderr, "         Since branch target address %s is too large or is negative,\n", targetAddrStr.c_str());
                    fprintf(stderr, "         Ignore label generation for this instruction\n");
                    current = current + "    # label generation is ignored for this instruction";
                }
                else {
                    maxPc = std::max(static_cast<unsigned>(targetAddr), maxPc);
                    if (!labelTable.count(targetAddr)) {
                        char temp[1024];
                        sprintf(temp, "%d", labelCount);
                        ++labelCount;
                        std::string newLabel = "L" + std::string(temp);
                        labelTable.insert(std::make_pair(targetAddr, newLabel));
                    }
                    current = std::string(op) + " " + std::string(rs) + " " + std::string(rt) + " " +
                              labelTable[targetAddr];
                }
            }
            else if (current.find("bgtz ") != std::string::npos) {
                char op[1024], rs[1024], c[1024];
                sscanf(current.c_str(), "%s%s%s", op, rs, c);
                int targetAddr;
                if (useHex) {
                    sscanf(c, "%x", &targetAddr);
                }
                else {
                    sscanf(c, "%d", &targetAddr);
                }
                targetAddr = toSigned(static_cast<unsigned>(targetAddr), 16);
                targetAddr = (targetAddr * 4 + 4) >> 2;
                targetAddr += i;
                std::string targetAddrStr;
                if (useHex) {
                    targetAddrStr = toHexString(targetAddr);
                }
                else {
                    targetAddrStr = toDecString(targetAddr);
                }
                if (targetAddr + 1 > MAXN || targetAddr < 0) {
                    fprintf(stderr, "Warning: Instruction #%d: %s\n", i + 1, current.c_str());
                    fprintf(stderr, "         Since branch target address %s is too large or is negative,\n", targetAddrStr.c_str());
                    fprintf(stderr, "         Ignore label generation for this instruction\n");
                    current = current + "    # label generation is ignored for this instruction";
                }
                else {
                    maxPc = std::max(static_cast<unsigned>(targetAddr), maxPc);
                    if (!labelTable.count(targetAddr)) {
                        char temp[1024];
                        sprintf(temp, "%d", labelCount);
                        ++labelCount;
                        std::string newLabel = "L" + std::string(temp);
                        labelTable.insert(std::make_pair(targetAddr, newLabel));
                    }
                    current = std::string(op) + " " + std::string(rs) + " " + labelTable[targetAddr];
                }
            }
            else if (current.find("j ") != std::string::npos ||
                     current.find("jal ") != std::string::npos) {
                char op[1024], c[1024];
                sscanf(current.c_str(), "%s%s", op, c);
                int targetAddr;
                if (useHex) {
                    sscanf(c, "%x", &targetAddr);
                }
                else {
                    sscanf(c, "%d", &targetAddr);
                }
                std::string targetAddrStr;
                if (useHex) {
                    targetAddrStr = toHexString(targetAddr);
                }
                else {
                    targetAddrStr = toDecString(targetAddr);
                }
                if (targetAddr + 1 > MAXN || targetAddr < 0) {
                    fprintf(stderr, "Warning: Instruction #%d: %s\n", i + 1, current.c_str());
                    fprintf(stderr, "         Since branch target address %s is too large or is negative,\n", targetAddrStr.c_str());
                    fprintf(stderr, "         Ignore label generation for this instruction\n");
                    current = current + "    # label generation is ignored for this instruction";
                }
                else {
                    maxPc = std::max(static_cast<unsigned>(targetAddr), maxPc);
                    if (!labelTable.count(targetAddr)) {
                        char temp[1024];
                        sprintf(temp, "%d", labelCount);
                        ++labelCount;
                        std::string newLabel = "L" + std::string(temp);
                        labelTable.insert(std::make_pair(targetAddr, newLabel));
                    }
                    current = std::string(op) + " " + labelTable[targetAddr];
                }
            }
        }
        if (maxPc + 1 > MAXN) {
            fprintf(stderr, "Warning: Some target addresses of branch instructions are too large.\n");
            fprintf(stderr, "         Try to use -nolabel to disable label auto generation.\n");
            exit(EXIT_FAILURE);
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

std::string InstDisassembler::getLine(const unsigned& line) const {
    if (line >= len) {
        return "";
    }
    return result[line];
}

}
