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

struct InstEncodeData {
    unsigned inst;
    bool isValid;
    InstEncodeData() :
            inst(0xFFFFFFFFu), isValid(false) {}
    InstEncodeData(const unsigned& inst, const bool& isValid) :
            inst(inst), isValid(isValid) {}
};

class InstEncoder {

public:
    InstEncoder();
    virtual ~InstEncoder();
    void init();
    void setPc(const unsigned& pc);
    void preProcess(std::string &src);
    InstEncodeData encodeInst(const std::string& inst);

private:
    InstEncodeData analyzeString(std::string inst);
    unsigned getReg(const std::string &src);
    unsigned getC(const std::string& src);
    unsigned getBranchC(const std::string& src);
    InstType getInstType(const std::string& inst);
    bool hasLabel(const std::string& src);
    bool isComment(const std::string& src);
    std::string nextString(std::string& src);
    std::string opToLower(const std::string &src);
    std::string toLowerString(const std::string &src);
    std::string processInputString(const std::string &inst);
    std::string trimWhiteSpace(const std::string& src);
    std::string trimLeadingWhiteSpace(const std::string &src);

private:
    int line;
    unsigned pc;
    unsigned preprocessPc;
    std::map<std::string, int> labelTable;
};

} /* namespace lb */


#endif /* INSTENCODER_H_ */
