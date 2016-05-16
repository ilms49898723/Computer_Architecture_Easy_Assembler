/*
 * InstAssembler.hpp
 *
 *  Created on: 2016/04/13
 *      Author: LittleBird
 */

#ifndef COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTASSEMBLER_HPP_
#define COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTASSEMBLER_HPP_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "InstEncoder.hpp"
#include "InstUtility.hpp"

namespace inst {

class InstAssembler {
public:
    InstAssembler();

    void init(const std::string& filename);

    void setInitialPc(const unsigned& initialPc);

    void insert(const std::string& inst);

    void start();

private:
    unsigned initialPc;
    bool valid;
    InstEncoder instEncoder;
    std::string filename;
    std::vector<std::string> inputAssembly;
    std::vector<unsigned> binary;
};

} /* namespace inst */

#endif /* COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTASSEMBLER_HPP_ */
