/*
 * InstDataBin.h
 *
 *  Created on: 2016/04/12
 *      Author: LittleBird
 */

#ifndef INSTENCODER_H_
#define INSTENCODER_H_

#include <cstdio>
#include <string>
#include <map>
#include <utility>
#include "InstLookUp.h"

namespace lb {

class InstEncoder {
private:
    struct InstDataEncode;

public:
    InstEncoder();
    virtual ~InstEncoder();
    void init();
    void init(const unsigned& pc);
    unsigned encodeInst(std::string inst);

private:
    InstDataEncode analyzeString(std::string inst);
    bool hasLabel(std::string src);

private:
    unsigned pc;
    std::map<std::string, int> labelTable;
};

} /* namespace lb */


#endif /* INSTENCODER_H_ */
