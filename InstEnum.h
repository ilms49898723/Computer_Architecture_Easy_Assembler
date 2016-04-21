/*
 * InstEnum.h
 *
 *  Created on: 2016/03/25
 *      Author: LittleBird
 */

#ifndef INSTENUM_H_
#define INSTENUM_H_

namespace lb {

// enum class for instruction type
// R-type, I-type, J-type, Specialized, Undefined
enum class InstType : unsigned {
    R, I, J, S, Undef
};

} /* namespace lb */

#endif /* INSTENUM_H_ */
