/*
 * InstLookUp.h
 *
 *  Created on: 2016/03/09
 *      Author: LittleBird
 */

#ifndef INSTLOOKUP_H_
#define INSTLOOKUP_H_

#include <string>
#include "InstUtility.h"

namespace lb {

class InstLookUp {
public:
    // translate opCode -> readable string (ex. addi)
    static std::string opCodeLookUp(const unsigned& src);

    // translate readable string to opcode binary code
    static unsigned translateToOpCode(const std::string& src);

    // translate funct -> readable string (ex. add);
    static std::string functLookUp(const unsigned& src);

    // translate readable string to funct binary code
    static unsigned translateToFunct(const std::string& src);

    // translate reg -> numbers (ex. $0~$31)
    static std::string registerLookUpNumber(const unsigned& src);

    // translate reg -> readable string (ex. t0, a0)
    static std::string registerLookUpName(const unsigned& src);

    // translate readable string to reg number
    static unsigned translateToReg(const std::string& src);

private:
    const static std::string opCodeLookUpTable[];
    const static std::string functLookUpTable[];
    const static std::string registerLookUpTable[];
};

} /* namespace lb */

#endif /* INSTLOOKUP_H_ */
