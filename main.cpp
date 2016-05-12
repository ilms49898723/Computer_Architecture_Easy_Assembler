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

int main(int argc, char** argv) {
    if (argc == 2 && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        printf("usage: %s [options] mode InputFile -o OutputFile\n", argv[0]);
        printf("mode:\n");
        printf("          -a: assembler\n");
        printf("          -d: disassembler\n");
        printf("options:\n");
        printf("in mode assembler\n");
        printf("   -pc value: specify initial pc value(default 0)\n");
        printf("in mode disassembler\n");
        printf("        -dec: use decimal for all constant integers(default)\n");
        printf("        -hex: use hexdecimal for all constant integers\n");
        printf("      -label: use label in branch instructions(ex. beq)(default)\n");
        printf("    -nolabel: don\'t use label in branch instructions(ex. beq)\n");
        printf("\n");
        printf("about constant values:\n");
        printf("in mode assembler, numbers with prefix \"0x\" will be a hexdecimal number\n");
        printf("    addi $1, $1, 15 -> 15 is decimal\n");
        printf("    addi $1, $1, 0x15 -> 0x15 is hexdecimal\n");
        printf("\n");
        exit(EXIT_SUCCESS);
    }
    lb::AssemblerArgumentInfo argu = lb::processArguments(argc, argv);
    if (!argu.isValid) {
        fprintf(stderr, "Invalid Arguments.\nUse %s -h or --help for more information.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argu.hasD) {
        FILE* fin = nullptr;
        fin = fopen(argu.inputFile.c_str(), "rb");
        if (!fin) {
            fprintf(stderr, "%s: %s\n", argu.inputFile.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        unsigned len, pc;
        unsigned inst[4096];
        len = lb::InstImageReader::readImageI(fin, inst, &pc, 4096);
        lb::InstDisassembler disassembler;
        disassembler.setUseLabel(!argu.hasNoLabel);
        disassembler.setUseHex(argu.useHex);
        disassembler.init(inst, len, pc);
        disassembler.start();
        FILE* fout = nullptr;
        fout = fopen(argu.outputFile.c_str(), "w");
        if (!fout) {
            fprintf(stderr, "%s: %s\n", argu.outputFile.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        for (unsigned i = 0; i < disassembler.length(); ++i) {
            fprintf(fout, "%s\n", disassembler.getLine(i).c_str());
        }
        fclose(fin);
        fclose(fout);
        exit(EXIT_SUCCESS);
    }
    if (argu.hasA) {
        FILE* fin = nullptr;
        fin = fopen(argu.inputFile.c_str(), "rt");
        if (!fin) {
            fprintf(stderr, "%s: %s\n", argu.inputFile.c_str(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        unsigned initialPc = 0;
        if (argu.hasInitPc) {
            initialPc = static_cast<unsigned>(argu.initPc);
        }
        if (initialPc % 4) {
            fprintf(stderr, "%d: pc value must be divisible by 4.\n", initialPc);
            exit(EXIT_FAILURE);
        }
        lb::InstAssembler assembler;
        assembler.init(argu.outputFile);
        assembler.setInitialPc(initialPc);
        char inputBuffer[4096];
        while (fgets(inputBuffer, 4096 - 1, fin)) {
            if (inputBuffer[strlen(inputBuffer) - 1] == '\n') {
                inputBuffer[strlen(inputBuffer) - 1] = '\0';
            }
            assembler.insert(inputBuffer);
        }
        assembler.start();
        fclose(fin);
        exit(EXIT_SUCCESS);
    }
}
