#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include "bitsReader.h"
#include "InstructionParser.h"

class ElfReader {
public:
    ElfReader(std::ifstream &in_, FILE *out);

    void process();

private:
    std::string isValid();

    void parseSectionHeader(uint index);

    void parseSectionHeaders();

    uint findTabIndex(const std::string &sectionName);

    std::string getSectionName(uint offset, uint sectionIndex);

    void parseSymbol(uint index);

    void parseSymbols();

    void processText();

    void printSymbol(uint index);

    void printSymbols();

    struct SectionHeader {
        uint sh_name;
        uint sh_offset;
        uint sh_size;
        uint sh_addr;

        std::string name;
    };

    struct Symbol {
        uint st_name;
        uint st_value;
        uint st_size;
        uchar st_info;
        uchar st_other;
        uint16_t st_shndx;

        std::string name;

        inline std::string getTypeName() {
            auto type = ELF32_ST_TYPE(st_info);
            if (type == STT_NOTYPE) return "NOTYPE";
            if (type == STT_OBJECT) return "OBJECT";
            if (type == STT_FUNC) return "FUNC";
            if (type == STT_SECTION) return "SECTION";
            if (type == STT_FILE) return "FILE";
            if (type == STT_COMMON) return "COMMON";
            if (type == STT_TLS) return "TLS";
            if (type == STT_LOOS) return "LOOS";
            if (type == STT_HIOS) return "HIOS";
            if (type == STT_LOPROC) return "LOPROC";
            if (type == STT_HIPROC) return "HIPROC";

            return "UNKNOWN";
        }

        inline std::string getBindName() {
            auto type = ELF32_ST_BIND(st_info);
            if (type == STB_LOCAL) return "LOCAL";
            if (type == STB_GLOBAL) return "GLOBAL";
            if (type == STB_WEAK) return "WEAK";
            if (type == STB_LOOS) return "LOOS";
            if (type == STB_HIOS) return "HIOS";
            if (type == STB_LOPROC) return "LOPROC";
            if (type == STB_HIPROC) return "HIPROC";

            return "UNKNOWN";
        }

        inline std::string getVisName() {
            auto type = ELF32_ST_VISIBILITY(st_other);
            if (type == STV_DEFAULT) return "DEFAULT";
            if (type == STV_INTERNAL) return "INTERNAL";
            if (type == STV_HIDDEN) return "HIDDEN";
            if (type == STV_PROTECTED) return "PROTECTED";

            return "UNKNOWN";
        }

        inline std::string getIndexName() {
            auto type = st_shndx;
            if (type == SHN_UNDEF) return "UNDEF";
            if (type == SHN_LORESERVE) return "LORESERVE";
            if (type == SHN_LOPROC) return "LOPROC";
            if (type == SHN_HIPROC) return "HIPROC";
            if (type == SHN_LOOS) return "LOOS";
            if (type == SHN_HIOS) return "HIOS";
            if (type == SHN_ABS) return "ABS";
            if (type == SHN_COMMON) return "COMMON";
            if (type == SHN_XINDEX) return "XINDEX";
            if (type == SHN_HIRESERVE) return "HIRESERVE";

            return std::to_string(type);
        }
    };

    FILE *out;
    BitsReader byteReader = BitsReader({});
    InstructionParser instructionParser;
    std::vector<SectionHeader> sectionHeaders;
    std::vector<Symbol> symbols;

    uint e_shoff;
    uint e_shnum;
    uint e_shstrndx;

    uint e_symtabndx;
    uint e_strtabndx;

    SectionHeader textSectionHeader;
};

