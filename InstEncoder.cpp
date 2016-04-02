//
// Created by LittleBird on 2016/04/02.
//

#include "InstEncoder.h"

namespace lb {

struct InstEncoder::InstDataEncode {
    std::string opCode;
    std::string rs, rt, rd;
    std::string c;
    std::string funct;
    InstDataEncode() {
        opCode = "";
        rs = "";
        rt = "";
        rd = "";
        c = "";
        funct = "";
    }
};

InstEncoder::InstEncoder() {
}

InstEncoder::~InstEncoder() {
}


void InstEncoder::init() {
    labelTable.clear();
    pc = 0;
}

void InstEncoder::init(const unsigned &pc) {
    labelTable.clear();
    this->pc = pc;
}

unsigned InstEncoder::encodeInst(std::string inst) {
    InstEncoder::InstDataEncode data = analyzeString(inst);
    pc += 4; // pc offset
    return 0;
}

InstEncoder::InstDataEncode InstEncoder::analyzeString(std::string inst) {
    // deal with comma ','
    for (unsigned i = 0; i < inst.length(); ++i) {
        if (inst[i] == ',') {
            inst[i] = ' ';
        }
    }
    // check whether this line contains Label like "ELSE: "
    if (hasLabel(inst)) {
        for (unsigned i = 0; i < inst.length(); ++i) {
            if (inst[i] == ':') {
                std::string label = inst.substr(0, i + 1);
                labelTable.insert(make_pair(label, pc));
            }
        }
    }
    // search op Code

    return InstDataEncode();
}

bool InstEncoder::hasLabel(std::string src) {
    return src.find(":", 0) != std::string::npos;
}

}
