/*
 * InstAssembler.cpp
 *
 *  Created on: 2016/04/13
 *      Author: LittleBird
 */

#include "InstAssembler.h"

namespace lb {

InstAssembler::InstAssembler() {
    fp = nullptr;
    initialPc = 0u;
    inputAssembly.clear();
    binary.clear();
}

void InstAssembler::init(FILE* fp) {
    InstAssembler::fp = fp;
    instEncoder.init();
}

void InstAssembler::setInitialPc(const unsigned& initialPc) {
    InstAssembler::initialPc = initialPc;
    instEncoder.setPc(initialPc);
}

void InstAssembler::insert(const std::string &inst) {
    inputAssembly.push_back(inst);
}

void InstAssembler::start() {
    fwriteUnsigned(fp, initialPc);
    for (unsigned i = 0; i < inputAssembly.size(); ++i) {
        instEncoder.preProcess(inputAssembly[i]);
    }
    for (unsigned i = 0; i < inputAssembly.size(); ++i) {
        InstEncodeData ret = instEncoder.encodeInst(inputAssembly[i]);
        binary.push_back(ret.inst);
    }
    fwriteUnsigned(fp, static_cast<unsigned>(binary.size()));
    for (unsigned i = 0; i < binary.size(); ++i) {
        fwriteUnsigned(fp, binary[i]);
    }
}

}

