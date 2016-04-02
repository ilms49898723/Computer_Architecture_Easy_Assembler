/*
 * main.cpp
 *
 *  Created on: 2016/03/26
 *      Author: LittleBird
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <string>
#include <vector>
#include <map>
#include "InstUtility.h"
#include "InstEncoder.h"
#include "InstDecoder.h"
#include "InstImageReader.h"

inline void writeUnsigned(FILE* fout, const unsigned& src);

int main(int argc, char **argv) {
    if (argc == 2 && std::string(argv[1]) == "-h") {
        printf("-a for assembler, -d for disassembler\n");
        printf("ex.\n");
        printf("    %s -a InputFilePath -o OutputFilePath\n", argv[0]);
        printf("    %s -d InputFilePath -o OutputFilePath\n", argv[0]);
        printf("option:\n-nolabel: don\'t print label in branch instructions like beq\n");
        printf("ex.\n    beq $t0, $t0, LABEL1 --> beq $t0, $t0, 0x0");
        printf("\n");
        exit(EXIT_SUCCESS);
    }
    lb::AssemblerArgumentInfo argu = lb::processArguments(argc, argv);
    if (!argu.isValid) {
        fprintf(stderr, "Invalid Arguments.\n%s -h for more information.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    printf("--Info--\n");
    printf("Mode: %s\n", (argu.hasD) ? "Disassembler" : "Assembler");
    printf("InputFile: %s\n", (argu.hasInputFile) ? argu.inputFile.c_str() : "N/A");
    printf("OutputFile: %s\n", (argu.hasOutputFile) ? argu.outputFile.c_str() : "N/A");
    if (argu.hasD) {
        FILE* fout = nullptr;
        fout = fopen(argu.outputFile.c_str(), "w");
        if (!fout) {
            fprintf(stderr, "File Open Error!\n");
            exit(EXIT_FAILURE);
        }
        std::vector<std::string> assembly;
        std::map<int, std::string> labelTable;
        int labelCount = 0;
        unsigned len, pc;
        unsigned inst[1024];
        len = lb::InstImageReader::readImageI(argu.inputFile.c_str(), inst, &pc);
        for (unsigned i = 0; i < len; ++i) {
            assembly.push_back(lb::InstDecoder::decodeInstStr(inst[i]).toString());
        }
        if (!argu.hasNoLabel) {
            for (unsigned i = 0; i < assembly.size(); ++i) {
                std::string &current = assembly[i];
                if (current.find("beq") != std::string::npos ||
                    current.find("bne") != std::string::npos) {
                    char op[1024], rs[1024], rt[1024], c[1024];
                    sscanf(current.c_str(), "%s%s%s%s", op, rs, rt, c);
                    int offset;
                    sscanf(c, "%x", &offset);
                    offset = lb::toSigned(static_cast<unsigned>(offset), 16);
                    offset = (offset * 4 + 4) >> 2;
                    if (!labelTable.count(i + offset)) {
                        char temp[1024];
                        sprintf(temp, "%d", labelCount);
                        ++labelCount;
                        std::string newLabel = "L" + std::string(temp);
                        labelTable.insert(std::make_pair(i + offset, newLabel));
                    }
                    current = std::string(op) + " " + std::string(rs) + " " + std::string(rt) + " " +
                              labelTable[i + offset];
                }
                else if (current.find("bgtz") != std::string::npos) {
                    char op[1024], rs[1024], c[1024];
                    sscanf(current.c_str(), "%s%s%s", op, rs, c);
                    int offset;
                    sscanf(c, "%x", &offset);
                    offset = lb::toSigned(static_cast<unsigned>(offset), 16);
                    offset = (offset * 4 + 4) >> 2;
                    if (!labelTable.count(i + offset)) {
                        char temp[1024];
                        sprintf(temp, "%d", labelCount);
                        ++labelCount;
                        std::string newLabel = "L" + std::string(temp);
                        labelTable.insert(std::make_pair(i + offset, newLabel));
                    }
                    current = std::string(op) + " " + std::string(rs) + " " + labelTable[i + offset];
                }
                else if (current.find("j") != std::string::npos ||
                         current.find("jal") != std::string::npos) {
                    char op[1024], c[1024];
                    sscanf(current.c_str(), "%s%s", op, c);
                    int offset;
                    sscanf(c, "%x", &offset);
                    if (!labelTable.count(i + offset)) {
                        char temp[1024];
                        sprintf(temp, "%d", labelCount);
                        ++labelCount;
                        std::string newLabel = "L" + std::string(temp);
                        labelTable.insert(std::make_pair(i + offset, newLabel));
                    }
                    current = std::string(op) + " " + labelTable[i + offset];
                }
            }
        }
        for (unsigned i = 0; i < assembly.size(); ++i) {
            if (!argu.hasNoLabel) {
                if (labelTable.count(i)) {
                    for (unsigned long long j = labelTable[i].length(); j < 4; ++j) {
                        fprintf(fout, " ");
                    }
                    fprintf(fout, "%s: ", labelTable[i].c_str());
                }
                else {
                    fprintf(fout, "      ");
                }
            }
            fprintf(fout, "%s\n", assembly[i].c_str());
        }
        printf("Finished.\n");
        fclose(fout);
        exit(EXIT_SUCCESS);
    }
    else {
        FILE* fin = nullptr;
        FILE *fout = nullptr;
        fin = fopen(argu.inputFile.c_str(), "r");
        fout = fopen(argu.outputFile.c_str(), "w");
        if (!fin || !fout) {
            fprintf(stderr, "File Open Error!\n");
            exit(EXIT_FAILURE);
        }
        unsigned initialPc = 0;
        printf("Initial Value of PC: ");
        scanf("%x", &initialPc);
        writeUnsigned(fout, initialPc);
        char inputBuffer[2048];
        lb::InstEncoder instEncoder;
        std::vector<std::string> inputAssembly;
        std::vector<unsigned> binary;
        instEncoder.setPc(initialPc);
        while (fgets(inputBuffer, 2048, fin)) {
            if (inputBuffer[strlen(inputBuffer) - 1] == '\n') {
                inputBuffer[strlen(inputBuffer) - 1] = '\0';
            }
            inputAssembly.push_back(inputBuffer);
        }
        for (auto& i : inputAssembly) {
            instEncoder.preProcess(i);
        }
        for (auto&i : inputAssembly) {
            lb::InstEncodeData ret = instEncoder.encodeInst(i);
            binary.push_back(ret.inst);
        }
        writeUnsigned(fout, binary.size());
        for (const auto i : binary) {
            writeUnsigned(fout, i);
        }
        fclose(fin);
        fclose(fout);
        exit(EXIT_SUCCESS);
    }
}

inline void writeUnsigned(FILE *fout, const unsigned &src) {
    unsigned char buffer[4];
    buffer[0] = static_cast<unsigned char>((src & 0xFF000000u) >> 24);
    buffer[1] = static_cast<unsigned char>((src & 0x00FF0000u) >> 16);
    buffer[2] = static_cast<unsigned char>((src & 0x0000FF00u) >> 8);
    buffer[3] = static_cast<unsigned char>((src & 0x000000FFu));
    fwrite(buffer, sizeof(unsigned char), 4, fout);
}
