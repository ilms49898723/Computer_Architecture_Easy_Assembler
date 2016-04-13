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
#include "InstAssembler.h"
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
        lb::InstAssembler assembler;
        assembler.init(fout);
        assembler.setInitialPc(initialPc);
        char inputBuffer[2048];
        while (fgets(inputBuffer, 2048, fin)) {
            if (inputBuffer[strlen(inputBuffer) - 1] == '\n') {
                inputBuffer[strlen(inputBuffer) - 1] = '\0';
            }
            assembler.insert(inputBuffer);
        }
        assembler.start();
        fclose(fin);
        fclose(fout);
        exit(EXIT_SUCCESS);
    }
}
