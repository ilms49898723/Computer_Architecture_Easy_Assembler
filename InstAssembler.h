/*
 * InstAssembler.h
 *
 *  Created on: 2016/04/13
 *      Author: LittleBird
 */

#ifndef INSTASSEMBLER_H_
#define INSTASSEMBLER_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "InstEncoder.h"
#include "InstUtility.h"

namespace lb {

class InstAssembler {
public:

    InstAssembler();
    void init(FILE* fp);
    void setInitialPc(const unsigned& initialPc);
    void insert(const std::string& inst);
    void start();

private:
    FILE *fp;
    unsigned initialPc;
    InstEncoder instEncoder;
    std::vector<std::string> inputAssembly;
    std::vector<unsigned> binary;
};

} /* namespace lb */

#endif /* INSTASSEMBLER_H_ */