#include "ElfReader.h"

ElfReader::ElfReader(std::ifstream &in, FILE *out_) : out(out_), instructionParser(out_) {
    uchar tmpChar;
    Bytes source;
    while (in.read((char *) &tmpChar, 1)) {
        source.emplace_back(tmpChar);
    }
    instructionParser = InstructionParser(out);
    byteReader = BitsReader(move(source));
}

std::string ElfReader::isValid() {
    if (byteReader.getByte4(reversedELFMagicConstantOffset) != reversedELFMagicConstant)
        return "File is incorrect (magic number does not match).";

    if (byteReader[bitnessOffset] != bitnessIs32) return "File is not in 32 bit format.";
    if (byteReader[endianOffset] != endianIsLittle) return "File is not in little endian.";
    if (byteReader[currentVersionOffset] != isCurrentVersion)
        return "File is not in the original and current version of ELF.";
    if (byteReader.getByte2(targetISAOffset) != RISCVIsTargetISA) return "File's target ISA is not RISC-V.";

    return "";
}

void ElfReader::parseSectionHeaders() {
    e_shoff = byteReader.getByte4(e_shoff_offset);
    e_shnum = byteReader.getByte2(e_shnum_offset);
    e_shstrndx = byteReader.getByte2(e_shstrndx_offset);

    sectionHeaders.resize(e_shnum);

    parseSectionHeader(e_shstrndx);

    for (uint i = 0; i < e_shnum; i++) parseSectionHeader(i);
}

void ElfReader::parseSectionHeader(uint index) {
    uint currentOffset = e_shoff + index * sectionHeaderSize;
    SectionHeader &sectionHeader = sectionHeaders[index];

    sectionHeader.sh_name = byteReader.getByte4(currentOffset + sh_name_offset);
    sectionHeader.sh_addr = byteReader.getByte4(currentOffset + sh_addr_offset);
    sectionHeader.sh_offset = byteReader.getByte4(currentOffset + sh_offset_offset);
    sectionHeader.sh_size = byteReader.getByte4(currentOffset + sh_size_offset);

    sectionHeader.name = getSectionName(sectionHeader.sh_name, e_shstrndx);
}

std::string ElfReader::getSectionName(uint offset, uint sectionIndex) {
    offset += sectionHeaders[sectionIndex].sh_offset;
    std::string result;
    auto tmp = (char) byteReader.getByte(offset++);
    while (tmp) {
        result += tmp;
        tmp = (char) byteReader.getByte(offset++);
    }
    return result;
}

uint ElfReader::findTabIndex(const std::string &sectionName) {
    for (int i = 0; i < e_shnum; i++) if (sectionHeaders[i].name == sectionName) return i;
    return -1;
}

void ElfReader::parseSymbol(uint index) {
    Symbol &symbol = symbols[index];
    uint currentOffset = index * symbolSizeBits + sectionHeaders[e_symtabndx].sh_offset;
    symbol.st_name = byteReader.getByte4(currentOffset);
    currentOffset += 4;
    symbol.st_value = byteReader.getByte4(currentOffset);
    currentOffset += 4;
    symbol.st_size = byteReader.getByte4(currentOffset);
    currentOffset += 4;
    symbol.st_info = byteReader.getByte(currentOffset);
    currentOffset += 1;
    symbol.st_other = byteReader.getByte(currentOffset);
    currentOffset += 1;
    symbol.st_shndx = byteReader.getByte2(currentOffset);

    symbol.name = getSectionName(symbol.st_name, e_strtabndx);

    if (ELF32_ST_TYPE(symbol.st_info) == STT_FUNC)
        if (!symbol.name.empty()) instructionParser.address2FunctionName[symbol.st_value] = symbol.name;
}

void ElfReader::parseSymbols() {
    uint amountOfSymbols = sectionHeaders[e_symtabndx].sh_size / symbolSizeBits;
    symbols.resize(amountOfSymbols);
    for (int i = 0; i < amountOfSymbols; i++) {
        parseSymbol(i);
    }
}

void ElfReader::processText() {
    uint index = 0;
    fprintf(out, ".text\n");
    while (index < textSectionHeader.sh_size) {
        instructionParser.parsInstruction(byteReader.getByte4(textSectionHeader.sh_offset + index),
                                          textSectionHeader.sh_addr + index);
        index += instructionSize;
    }
}

void ElfReader::printSymbol(uint index) {
    Symbol &symbol = symbols[index];
    fprintf(out, "[%4i] 0x%-15X %5i %-8s %-8s %-8s %6s %s\n", index, symbol.st_value, symbol.st_size,
            symbol.getTypeName().c_str(), symbol.getBindName().c_str(), symbol.getVisName().c_str(),
            symbol.getIndexName().c_str(),
            symbol.name.c_str());
}

void ElfReader::printSymbols() {
    uint amountOfSymbols = sectionHeaders[e_symtabndx].sh_size / symbolSizeBits;
    fprintf(out, "\n.symtab\nSymbol Value              Size Type     Bind     Vis       Index Name\n");
    symbols.resize(amountOfSymbols);
    for (int i = 0; i < amountOfSymbols; i++) {
        printSymbol(i);
    }
}

void ElfReader::process() {
    std::string error = isValid();
    if (!error.empty()) {
        std::cout << "Error!" + error;
        return;
    }

    parseSectionHeaders();

    e_strtabndx = findTabIndex(".strtab");
    e_symtabndx = findTabIndex(".symtab");
    textSectionHeader = sectionHeaders[findTabIndex(".text")];

    parseSymbols();

    processText();

    printSymbols();
}
