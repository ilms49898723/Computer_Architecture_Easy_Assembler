/*
 * InstUtility.cpp
 *
 *  Created on: 2016/03/10
 *      Author: LittleBird
 */

#include "InstUtility.h"

namespace lb {

bool isValidArguments(int& argc, char**& argv) {
    if (argc < 2) {
        return false;
    }
    bool hasA = false;
    bool hasD = false;
    bool hasNoLabel = false;
    bool hasInitPc = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-d") {
            hasD = true;
        }
        if (std::string(argv[i]) == "-a") {
            hasA = true;
        }
        if (std::string(argv[i]) == "-nolabel") {
            hasNoLabel = true;
        }
        if (std::string(argv[i]) == "-pc") {
            hasInitPc = true;
        }
    }
    if (hasA == hasD) {
        return false;
    }
    else if (hasA && hasNoLabel) {
        return false;
    }
    else if (hasD && hasInitPc) {
        return false;
    }
    else {
        return true;
    }
}

int fwriteUnsigned(FILE *fout, const unsigned &src) {
    unsigned char buffer[4];
    buffer[0] = static_cast<unsigned char>((src >> 24) & 0xFFu);
    buffer[1] = static_cast<unsigned char>((src >> 16) & 0xFFu);
    buffer[2] = static_cast<unsigned char>((src >> 8) & 0xFFu);
    buffer[3] = static_cast<unsigned char>(src & 0xFFu);
    return fwrite(buffer, sizeof(unsigned char), 4, fout);
}

AssemblerArgumentInfo processArguments(int& argc, char**& argv) {
    if (!isValidArguments(argc, argv)) {
        return AssemblerArgumentInfo();
    }
    AssemblerArgumentInfo ret;
    bool usedArguments[15];
    memset(usedArguments, false, sizeof(bool) * 15);
    for (int i = 1; i < 15 && i < argc; ++i) {
        if (std::string(argv[i]) == "-a") {
            ret.hasA = true;
            usedArguments[i] = true;
        }
        if (std::string(argv[i]) == "-d") {
            ret.hasD = true;
            usedArguments[i] = true;
        }
        if (std::string(argv[i]) == "-o") {
            ret.hasOutputFile = true;
            usedArguments[i] = true;
            if (i + 1 < argc) {
                ret.outputFile = argv[i + 1];
                usedArguments[i + 1] = true;
            }
            else {
                return AssemblerArgumentInfo();
            }
        }
        if (std::string(argv[i]) == "-pc") {
            ret.hasInitPc = true;
            usedArguments[i] = true;
            if (i + 1 < argc) {
                for (int j = 0; argv[i + 1][j]; ++j) {
                    if (!isdigit(argv[i + 1][j])) {
                        return AssemblerArgumentInfo();
                    }
                }
                if (sscanf(argv[i + 1], "%d", &ret.initPc) != 1) {
                    return AssemblerArgumentInfo();
                }
                usedArguments[i + 1] = true;
            }
            else {
                return AssemblerArgumentInfo();
            }
        }
        if (std::string(argv[i]) == "-nolabel") {
            ret.hasNoLabel = true;
            usedArguments[i] = true;
        }
        if (std::string(argv[i]) == "-hex") {
            ret.useHex = true;
            usedArguments[i] = true;
        }
        if (std::string(argv[i]) == "-label" || std::string(argv[i]) == "-dec") {
            usedArguments[i] = true;
        }
    }
    int unused = 0;
    for (int i = 1; i < argc; ++i) {
        unused += !usedArguments[i];
    }
    if (unused > 1) {
        return AssemblerArgumentInfo();
    }
    for (int i = 1; i < argc; ++i) {
        if (!usedArguments[i]) {
            ret.hasInputFile = true;
            ret.inputFile = argv[i];
            usedArguments[i] = true;
        }
    }
    if (!ret.hasInputFile || !ret.hasOutputFile) {
        return AssemblerArgumentInfo();
    }
    ret.isValid = true;
    return ret;
}

int toSigned(const unsigned& src, const int& bits) {
    int var = static_cast<int>(src << (32 - bits));
    int ret = var >> (32 - bits);
    return ret;
}

unsigned toUnsigned(const int& src) {
    return static_cast<unsigned>(src);
}

unsigned getBitsInRange(const unsigned& src, const int& l, const int& r) {
    return (src << (32 - r)) >> (l + (32 - r));
}

} /* namespace lb */
