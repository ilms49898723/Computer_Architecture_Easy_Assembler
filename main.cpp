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
#include "InstDisassembler.h"
#include "InstImageReader.h"

int main(int argc, char **argv) {
    if (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        printf("usage: %s [option] mode InputFile -o OutputFile\n", argv[0]);
        printf("option:\n");
        printf("    -nolabel: when disassembling, don\'t use label in branch instructions(ex. beq)\n");
        printf("              only available in mode disassembler\n");
        printf("    -pc value: specify initial pc value\n");
        printf("               only available in mode assembler\n");
        printf("mode:\n");
        printf("-a: assembler\n");
        printf("-d: disassembler\n\n");
        printf("ex.\n");
        printf("    %s -a InputFile -o OutputFile\n", argv[0]);
        printf("    %s -d InputFile -o OutputFile\n", argv[0]);
        printf("\n");
        exit(EXIT_SUCCESS);
    }
    lb::AssemblerArgumentInfo argu = lb::processArguments(argc, argv);
    if (!argu.isValid) {
        fprintf(stderr, "Invalid Arguments.\nUse %s -h or --help for more information.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    printf("--Info--\n");
    printf("Mode: %s\n", (argu.hasA) ? "Assembler" : "Disassembler");
    printf("InputFile: %s\n", (argu.hasInputFile) ? argu.inputFile.c_str() : "N/A");
    printf("OutputFile: %s\n", (argu.hasOutputFile) ? argu.outputFile.c_str() : "N/A");
    printf("--Info--\n");
    if (argu.hasD) {
        FILE* fin = nullptr;
        fin = fopen(argu.inputFile.c_str(), "rb");
        if (!fin) {
            fprintf(stderr, "%s: %s\n", argu.inputFile.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        FILE* fout = nullptr;
        fout = fopen(argu.outputFile.c_str(), "w");
        if (!fout) {
            fprintf(stderr, "%s: %s\n", argu.outputFile.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        unsigned len, pc;
        unsigned inst[1024];
        len = lb::InstImageReader::readImageI(fin, inst, &pc);
        lb::InstDisassembler disassembler;
        disassembler.setUseLabel(!argu.hasNoLabel);
        disassembler.init(inst, len, pc);
        disassembler.start();
        for (unsigned i = 0; i < len; ++i) {
            printf("%s\n", disassembler.getLine(i).c_str());
        }
        fclose(fout);
        exit(EXIT_SUCCESS);
    }
    else {
        FILE* fin = nullptr;
        FILE *fout = nullptr;
        fin = fopen(argu.inputFile.c_str(), "rt");
        fout = fopen(argu.outputFile.c_str(), "wb");
        if (!fin) {
            fprintf(stderr, "%s: %s\n", argu.inputFile.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (!fout) {
            fprintf(stderr, "%s: %s\n", argu.outputFile.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        unsigned initialPc = 0;
        if (argu.hasInitPc) {
            initialPc = static_cast<unsigned>(argu.initPc);
        }
        lb::fwriteUnsigned(fout, initialPc);
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
        for (auto i = 0u; i < inputAssembly.size(); ++i) {
            std::string ret = instEncoder.preProcess(inputAssembly[i]);
        }
        for (auto&i : inputAssembly) {
            lb::InstEncodeData ret = instEncoder.encodeInst(i);
            binary.push_back(ret.inst);
        }
        lb::fwriteUnsigned(fout, static_cast<unsigned>(binary.size()));
        for (const auto& i : binary) {
            lb::fwriteUnsigned(fout, i);
        }
        fclose(fin);
        fclose(fout);
        exit(EXIT_SUCCESS);
    }
}
