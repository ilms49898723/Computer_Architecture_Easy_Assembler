/*
 * main.cpp
 *
 *  Created on: 2016/03/26
 *      Author: LittleBird
 */

#include <cstdio>
#include <cstdlib>
#include <string>
#include "InstDecode.h"
#include "InstImageReader.h"
#include "InstUtility.h"

int main(int argc, char **argv) {
    if (argc == 2 && std::string(argv[1]) == "-h") {
        printf("-a for assembler, -d for disassembler\n");
        printf("ex.\n");
        printf("    %s -a [InputFilePath] [-o outputFilePath]\n", argv[0]);
        printf("    %s -d [InputFilePath] [-o outputFilePath]\n", argv[0]);
        printf("\n");
        exit(EXIT_SUCCESS);
    }
    lb::AssemblerArgumentInfo argu = lb::processArguments(argc, argv);
    if (!argu.isValid) {
        fprintf(stderr, "Invalid Arguments.\nUse %s -h for more information.\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    printf("Info:\n");
    printf("Mode: %s\n", (argu.hasD) ? "Disassembler" : "Assembler");
    printf("InputFile: %s\n", (argu.hasInputFile) ? argu.inputFile.c_str() : "N/A");
    printf("OutputFile: %s\n", (argu.hasOutputFile) ? argu.outputFile.c_str() : "N/A");
    printf("\n");
    if (argu.hasD) {
        unsigned len, pc;
        unsigned inst[1024];
        len = lb::InstImageReader::readImageI(argv[2], inst, &pc);
        printf("length = %u, pc = %u\n", len, pc);
        printf("%d instructions to decode\n", len);
        for (unsigned i = 0; i < len; ++i) {
            printf("%s\n", lb::InstDecode::decodeInstStr(inst[i]).toString().c_str());
        }
        printf("finished\n");
        exit(EXIT_SUCCESS);
    }
    return 0;
}
