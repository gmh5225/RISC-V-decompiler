#include "InstructionParser.h"

void InstructionParser::fillInstructions() {
    uint index = 0;
    instructions[index++] = {"lui", 0b0110111, U};
    instructions[index++] = {"auipc", 0b0010111, U};
    instructions[index++] = {"jal", 0b1101111, J};
    instructions[index++] = {"jalr", 0b1100111, I_LOAD, 0b000};
    instructions[index++] = {"beq", 0b1100011, B, 0b000};
    instructions[index++] = {"bne", 0b1100011, B, 0b001};
    instructions[index++] = {"blt", 0b1100011, B, 0b100};
    instructions[index++] = {"bge", 0b1100011, B, 0b101};
    instructions[index++] = {"bltu", 0b1100011, B, 0b110};
    instructions[index++] = {"bgeu", 0b1100011, B, 0b111};
    instructions[index++] = {"lb", 0b0000011, I_LOAD, 0b000};
    instructions[index++] = {"lh", 0b0000011, I_LOAD, 0b001};
    instructions[index++] = {"lw", 0b0000011, I_LOAD, 0b010};
    instructions[index++] = {"lbu", 0b0000011, I_LOAD, 0b100};
    instructions[index++] = {"lhu", 0b0000011, I_LOAD, 0b101};
    instructions[index++] = {"sb", 0b0100011, S, 0b000};
    instructions[index++] = {"sh", 0b0100011, S, 0b001};
    instructions[index++] = {"sw", 0b0100011, S, 0b010};
    instructions[index++] = {"addi", 0b0010011, I, 0b000};
    instructions[index++] = {"slti", 0b0010011, I, 0b010};
    instructions[index++] = {"sltiu", 0b0010011, I, 0b011};
    instructions[index++] = {"xori", 0b0010011, I, 0b100};
    instructions[index++] = {"ori", 0b0010011, I, 0b110};
    instructions[index++] = {"andi", 0b0010011, I, 0b111};
    instructions[index++] = {"slli", 0b0010011, I, 0b001, 0b0000000};
    instructions[index++] = {"srli", 0b0010011, I, 0b101, 0b0000000};
    instructions[index++] = {"srai", 0b0010011, I, 0b101, 0b0100000};
    instructions[index++] = {"add", 0b0110011, R, 0b000, 0b0000000};
    instructions[index++] = {"sub", 0b0110011, R, 0b000, 0b0100000};
    instructions[index++] = {"sll", 0b0110011, R, 0b001, 0b0000000};
    instructions[index++] = {"slt", 0b0110011, R, 0b010, 0b0000000};
    instructions[index++] = {"sltu", 0b0110011, R, 0b011, 0b0000000};
    instructions[index++] = {"xor", 0b0110011, R, 0b100, 0b0000000};
    instructions[index++] = {"srl", 0b0110011, R, 0b101, 0b0000000};
    instructions[index++] = {"sra", 0b0110011, R, 0b101, 0b0100000};
    instructions[index++] = {"or", 0b0110011, R, 0b110, 0b0000000};
    instructions[index++] = {"and", 0b0110011, R, 0b111, 0b0000000};
    instructions[index++] = {"ecall", 0b1110011, ECALL_EBREAK, 0b000, 0b0000000, 0b0};
    instructions[index++] = {"ebreak", 0b1110011, ECALL_EBREAK, 0b000, 0b0000000, 0b1};
    instructions[index++] = {"mul", 0b0110011, R, 0b000, 0b0000001};
    instructions[index++] = {"mulh", 0b0110011, R, 0b001, 0b0000001};
    instructions[index++] = {"mulhsu", 0b0110011, R, 0b010, 0b0000001};
    instructions[index++] = {"mulhu", 0b0110011, R, 0b011, 0b0000001};
    instructions[index++] = {"div", 0b0110011, R, 0b100, 0b0000001};
    instructions[index++] = {"divu", 0b0110011, R, 0b101, 0b0000001};
    instructions[index++] = {"rem", 0b0110011, R, 0b110, 0b0000001};
    instructions[index++] = {"remu", 0b0110011, R, 0b111, 0b0000001};
}

InstructionParser::InstructionParser(FILE *out_) {
    out = out_;
    fillInstructions();
}

void
InstructionParser::getTypeAndProcess(Byte4 instruction, InstructionType type, const std::string &name, uint address) {
    if (type == R) processRType(instruction, name);
    if (type == I or type == I_LOAD) processIType(instruction, name, type);
    if (type == S) processSType(instruction, name);
    if (type == B) processBType(instruction, name, address);
    if (type == U) processUType(instruction, name);
    if (type == J) processJType(instruction, name, address);
    if (type == ECALL_EBREAK) processECALL_EBREAKType(name);
    fprintf(out, "\n");
}

bool InstructionParser::checkSuccess(Byte4 instruction, std::vector<Instruction> &availableInstructions, uint address) {
    if (availableInstructions.empty()) {
        processUnknown();
        return true;
    }

    if (availableInstructions.size() == 1) {
        getTypeAndProcess(instruction, availableInstructions[0].instructionType, availableInstructions[0].name,
                          address);
        return true;
    }

    return false;
}

void InstructionParser::parsInstruction(Byte4 instruction, uint address) {
    if (address2FunctionName.count(address))
        fprintf(out, "%08x   <%s>:\n", address,
                address2FunctionName[address].c_str());
    fprintf(out, "   %05x:\t%08x\t", address,
            instruction);


    std::vector<Instruction> accessibleInstructions1;
    std::bitset<7> opcode = getBits<6, 0, 32>(instruction);
    for (auto &com: instructions) {
        if (com.opcode == opcode) accessibleInstructions1.push_back(com);
    }

    if (checkSuccess(instruction, accessibleInstructions1, address)) return;

    std::vector<Instruction> accessibleInstructions2;
    std::bitset<3> func3 = getBits<14, 12, 32>(instruction);
    for (auto &com: accessibleInstructions1) {
        if (com.func3 == func3) accessibleInstructions2.push_back(com);
    }

    if (checkSuccess(instruction, accessibleInstructions2, address)) return;

    std::vector<Instruction> accessibleInstructions3;
    std::bitset<7> func7 = getBits<31, 25, 32>(instruction);
    for (auto &com: accessibleInstructions2) {
        if (com.func7 == func7) accessibleInstructions3.push_back(com);
    }

    if (checkSuccess(instruction, accessibleInstructions3, address)) return;

    std::vector<Instruction> accessibleInstructions4;
    std::bitset<1> bit20 = getBits<20, 20, 32>(instruction);
    for (auto &com: accessibleInstructions3) {
        if (com.bit20 == bit20) accessibleInstructions4.push_back(com);
    }

    if (checkSuccess(instruction, accessibleInstructions4, address)) return;
}

void InstructionParser::processRType(Byte4 instruction, const std::string &name) {
    std::string rs1 = getRegName(getBits<19, 15, 32>(instruction));
    std::string rs2 = getRegName(getBits<24, 20, 32>(instruction));
    std::string rd = getRegName(getBits<11, 7, 32>(instruction));

    fprintf(out, "%7s\t%s, %s, %s", name.c_str(), rd.c_str(), rs1.c_str(), rs2.c_str());
}

void InstructionParser::processIType(Byte4 instruction, const std::string &name, InstructionType type) {
    std::string rs1 = getRegName(getBits<19, 15, 32>(instruction));
    uint imm;
    if (name == "slli" or name == "srli" or name == "srai") imm = getBits<24, 20, 32>(instruction).to_ulong();
    else imm = convert2signed<12>(getBits<31, 20, 32>(instruction));
    std::string rd = getRegName(getBits<11, 7, 32>(instruction));
    fprintf(out, "%7s\t%s, ", name.c_str(), rd.c_str());
    if (type == I) fprintf(out, "%s, %d", rs1.c_str(), imm);
    if (type == I_LOAD) fprintf(out, "%d(%s)", imm, rs1.c_str());
}

void InstructionParser::processSType(Byte4 instruction, const std::string &name) {
    std::string rs1 = getRegName(getBits<19, 15, 32>(instruction));
    std::string rs2 = getRegName(getBits<24, 20, 32>(instruction));

    std::bitset<7> offset11_5 = getBits<31, 25, 32>(instruction);
    std::bitset<5> offset4_0 = getBits<11, 7, 32>(instruction);
    int offset = convert2signed<12>(concat<7, 5>(offset11_5, offset4_0));

    fprintf(out, "%7s\t%s, %d(%s)", name.c_str(), rs2.c_str(), offset, rs1.c_str());
}

void InstructionParser::processBType(Byte4 instruction, const std::string &name, uint address) {
    std::string rs1 = getRegName(getBits<19, 15, 32>(instruction));
    std::string rs2 = getRegName(getBits<24, 20, 32>(instruction));

    std::bitset<1> offset12 = getBits<31, 31, 32>(instruction);
    std::bitset<6> offset10_5 = getBits<30, 25, 32>(instruction);
    std::bitset<4> offset4_1 = getBits<11, 8, 32>(instruction);
    std::bitset<1> offset11 = getBits<7, 7, 32>(instruction);
    int32_t offset = convert2signed<13>(
            concat<12, 1>(concat<1, 1, 6, 4>(offset12, offset11, offset10_5, offset4_1), 0));

    fprintf(out, "%7s\t%s, %s, 0x%0x", name.c_str(), rs1.c_str(), rs2.c_str(), offset + address);
    printRelevantObjectName(offset + address);
}

void InstructionParser::processUType(Byte4 instruction, const std::string &name) {
    std::string rd = getRegName(getBits<11, 7, 32>(instruction));
    uint imm = getBits<31, 12, 32>(instruction).to_ulong();

    fprintf(out, "%7s\t%s, %d", name.c_str(), rd.c_str(), imm);
}

void InstructionParser::processJType(Byte4 instruction, const std::string &name, uint address) {
    std::string rd = getRegName(getBits<11, 7, 32>(instruction));
    std::bitset<20> imm = getBits<31, 32 - 20, 32>(instruction);
    std::bitset<1> offset20 = getBits<19, 19, 20>(imm);
    std::bitset<8> offset19_12 = getBits<7, 0, 20>(imm);
    std::bitset<1> offset11 = getBits<8, 8, 20>(imm);
    std::bitset<10> offset10_1 = getBits<18, 9, 20>(imm);

    int32_t offset = convert2signed<21>(
            concat<20, 1>(concat<1, 8, 1, 10>(offset20, offset19_12, offset11, offset10_1), 0));

    fprintf(out, "%7s\t%s, 0x%0x", name.c_str(), rd.c_str(), offset + address);
    printRelevantObjectName(offset + address);
}

void InstructionParser::processECALL_EBREAKType(const std::string &name) {
    fprintf(out, " %7s", name.c_str());
}

void InstructionParser::processUnknown() {
    fprintf(out, "   %7s\n", unknownInstructionName.c_str());
}

std::string InstructionParser::getRegName(Reg rg) {
    uchar reg = rg.to_ulong();
    if (reg == 0) return "zero";
    if (reg == 1) return "ra";
    if (reg == 2) return "sp";
    if (reg == 3) return "gp";
    if (reg == 4) return "tp";
    if (reg >= 5 && reg <= 7) return "t" + std::to_string(reg - 5);
    if (reg >= 8 && reg <= 9) return "s" + std::to_string(reg - 8);
    if (reg >= 10 && reg <= 17) return "a" + std::to_string(reg - 10);
    if (reg >= 18 && reg <= 27) return "s" + std::to_string(reg - 16);
    if (reg >= 28 && reg <= 31) return "t" + std::to_string(reg - 25);
    return "";
}

void InstructionParser::printRelevantObjectName(uint address) {
    if (address2FunctionName.count(address)) fprintf(out, " <%s>", address2FunctionName[address].c_str());
    else fprintf(out, " <%s>", (address2FunctionName[address] = "L" + std::to_string(amountOfLocalObjects++)).c_str());
}
