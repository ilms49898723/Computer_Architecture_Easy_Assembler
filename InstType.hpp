/*
 * InstType.hpp
 *
 *  Created on: 2016/03/25
 *      Author: LittleBird
 */

#ifndef COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTTYPE_HPP_
#define COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTTYPE_HPP_

namespace inst {

// enum class for instruction type
// R-type, I-type, J-type, Specialized, Undefined
enum class InstType : unsigned {
    R, I, J, S, Undef
};

} /* namespace inst */

#endif /* COMPUTER_ARCHITECTURE_MINI_ASSEMBLER_INSTTYPE_HPP_ */
