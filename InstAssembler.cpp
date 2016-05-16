/*
 * InstAssembler.cpp
 *
 *  Created on: 2016/04/13
 *      Author: LittleBird
 */

#include "InstAssembler.hpp"

namespace inst {

InstAssembler::InstAssembler() {
    valid = true;
    initialPc = 0u;
    inputAssembly.clear();
    binary.clear();
}

void InstAssembler::init(const std::string& filename) {
    InstAssembler::filename = filename;
    instEncoder.init();
}

void InstAssembler::setInitialPc(const unsigned& initialPc) {
    InstAssembler::initialPc = initialPc;
    instEncoder.setPc(initialPc);
}

void InstAssembler::insert(const std::string& inst) {
    inputAssembly.push_back(inst);
}

void InstAssembler::start() {
    for (unsigned i = 0; i < inputAssembly.size(); ++i) {
        instEncoder.processLabel(inputAssembly[i]);
    }
    for (unsigned i = 0; i < inputAssembly.size(); ++i) {
        InstEncodeData ret = instEncoder.encodeInst(inputAssembly[i]);
        if (!ret.isValid && !ret.inst) {
            continue;
        }
        valid &= ret.isValid;
        binary.push_back(ret.inst);
        if (!valid) {
            binary.clear();
            break;
        }
    }
    if (!binary.empty()) {
        FILE* fp = fopen(filename.c_str(), "wb");
        if (!fp) {
            fprintf(stderr, "%s: %s\n", filename.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        fwriteUnsigned(fp, initialPc);
        fwriteUnsigned(fp, static_cast<unsigned>(binary.size()));
        for (unsigned i = 0; i < binary.size(); ++i) {
            fwriteUnsigned(fp, binary[i]);
        }
        fclose(fp);
    }
}

} /* namespace inst */
