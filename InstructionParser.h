#pragma once

#include "constants.h"
#include "bitsReader.h"
#include <unordered_map>
#include <array>
#include <string>
#include <fstream>

const uint amountOfInstructions = 47;

class InstructionParser {
public:
    InstructionParser(FILE *out);

    void parsInstruction(Byte4 instruction, uint address);

    std::unordered_map<uint, std::string> address2FunctionName;
private:
    enum InstructionType {
        R,
        I,
        I_LOAD,
        S,
        B,
        U,
        J,
        ECALL_EBREAK,
        UNKNOWN
    };

    struct Instruction {
        std::string name;
        std::bitset<7> opcode;
        InstructionType instructionType = UNKNOWN;
        std::bitset<3> func3;
        std::bitset<7> func7;

        std::bitset<1> bit20;
    };

    void fillInstructions();

    void getTypeAndProcess(Byte4 instruction, InstructionType type, const std::string &name, uint address);

    void processRType(Byte4 instruction, const std::string &name);

    void processIType(Byte4 instruction, const std::string &name, InstructionType type);

    void processSType(Byte4 instruction, const std::string &name);

    void processBType(Byte4 instruction, const std::string &name, uint address);

    void processUType(Byte4 instruction, const std::string &name);

    void processJType(Byte4 instruction, const std::string &name, uint address);

    void processECALL_EBREAKType(const std::string &name);

    void processUnknown();

    bool checkSuccess(Byte4 instruction, std::vector<Instruction> &availableInstructions, uint address);

    void printRelevantObjectName(uint address);

    static std::string getRegName(Reg rg);

    FILE *out;
    std::string unknownInstructionName = "unknown_instruction";
    std::array<Instruction, amountOfInstructions> instructions;

    uint amountOfLocalObjects = 0;
};
