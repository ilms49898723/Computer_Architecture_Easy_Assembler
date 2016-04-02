/*
 * InstEncoder.h
 *
 *  Created on: 2016/04/12
 *      Author: LittleBird
 */

#ifndef INSTENCODER_H_
#define INSTENCODER_H_

#include <cstdio>
#include <cctype>
#include <string>
#include <map>
#include <utility>
#include "InstLookUp.h"
#include "InstUtility.h"

namespace lb {

class InstEncoder {
private:
    struct InstDataEncode;

public:
    InstEncoder();
    virtual ~InstEncoder();
    void init();
    unsigned encodeInst(const std::string& inst);

private:
    unsigned analyzeString(std::string inst);
    InstType getInstType(const std::string& inst);
    unsigned getReg(const std::string &src);
    unsigned getC(const std::string& src);
    unsigned getBranchC(const std::string& src);
    bool hasLabel(const std::string& src);
    bool isComment(const std::string& src);
    std::string processInputString(const std::string &inst);
    std::string toLowerString(const std::string& src);
    std::string nextString(std::string& src);
    std::string trimWhiteSpace(const std::string& src);

private:
    int line;
    bool alive;
    unsigned pc;
    std::map<std::string, int> labelTable;
};

} /* namespace lb */


#endif /* INSTENCODER_H_ */
