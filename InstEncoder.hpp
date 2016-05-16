/*
 * InstEncoder.hpp
 *
 *  Created on: 2016/04/12
 *      Author: LittleBird
 */

#ifndef COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTENCODER_HPP_
#define COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTENCODER_HPP_

#include <cstdio>
#include <cctype>
#include <string>
#include <map>
#include <utility>
#include "InstLookUp.hpp"
#include "InstUtility.hpp"

namespace inst {

struct InstEncodeData {
    unsigned inst;
    bool isValid;

    InstEncodeData() :
            inst(0xFFFFFFFFu), isValid(false) { }

    InstEncodeData(const unsigned& inst, const bool& isValid) :
            inst(inst), isValid(isValid) { }
};

class InstEncoder {
public:
    InstEncoder();

    virtual ~InstEncoder();

    void init();

    void setPc(const unsigned& pc);

    void processLabel(std::string& src);

    InstEncodeData encodeInst(const std::string& inst);

private:
    void printErrorMessage(const std::string& msg, const int& idx);

    int splitInputString(const std::string& src);

    InstEncodeData analyzeString(const std::string& inst);

    unsigned getReg(const std::string& src, const int& cnt);

    unsigned getC(const std::string& src, const int& cnt);

    unsigned getBranchC(const std::string& src, const int& cnt);

    InstType getInstType(const std::string& inst);

    bool hasLabel(const std::string& src);

    bool isEmptyOrCommentLine(const std::string& src);

    bool isNumber(const std::string& src);

    bool checkElementsCount(const int& cnt);

    std::string getElements(const int& target);

    std::string nextString(std::string& src);

    std::string toLowerString(const std::string& src);

    std::string trimWhiteSpace(const std::string& src);

    std::string trimLeadingWhiteSpace(const std::string& src);

    std::string removeComment(const std::string& src);

private:
    bool valid;
    int line;
    unsigned pc;
    unsigned preprocessPc;
    std::string originalInputString;
    int elementsLength;
    int elementsLocation[15];
    std::string elements[15];
    std::map<std::string, int> labelTable;
};

} /* namespace inst */

#endif /* COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTENCODER_HPP_ */
