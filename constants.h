#pragma once

#include <bitset>

#define ELF32_ST_BIND(i)   ((i)>>4)
#define ELF32_ST_TYPE(i)   ((i)&0xf)
#define ELF32_ST_VISIBILITY(o) ((o)&0x3)

using uchar = unsigned char;
using uint = unsigned int;
using Byte = std::bitset<8>;
using Byte4 = std::bitset<32>;
using Bytes = std::vector<Byte>;

using Reg = std::bitset<5>;

const uint sectionHeaderSize = 0x28;

const uint SHT_SYMTAB = 0x2;
const uint SHT_STRTAB = 0x3;

const uchar STB_LOCAL = 0;
const uchar STB_GLOBAL = 1;
const uchar STB_WEAK = 2;
const uchar STB_LOOS = 10;
const uchar STB_HIOS = 12;
const uchar STB_LOPROC = 13;
const uchar STB_HIPROC = 15;

const uchar STT_NOTYPE = 0;
const uchar STT_OBJECT = 1;
const uchar STT_FUNC = 2;
const uchar STT_SECTION = 3;
const uchar STT_FILE = 4;
const uchar STT_COMMON = 5;
const uchar STT_TLS = 6;
const uchar STT_LOOS = 10;
const uchar STT_HIOS = 12;
const uchar STT_LOPROC = 13;
const uchar STT_HIPROC = 15;

const uchar STV_DEFAULT = 0;
const uchar STV_INTERNAL = 1;
const uchar STV_HIDDEN = 2;
const uchar STV_PROTECTED = 3;

const uint16_t SHN_UNDEF = 0;
const uint16_t SHN_LORESERVE = 0xff00;
const uint16_t SHN_LOPROC = 0xff00;
const uint16_t SHN_HIPROC = 0xff1f;
const uint16_t SHN_LOOS = 0xff20;
const uint16_t SHN_HIOS = 0xff3f;
const uint16_t SHN_ABS = 0xfff1;
const uint16_t SHN_COMMON = 0xfff2;
const uint16_t SHN_XINDEX = 0xffff;
const uint16_t SHN_HIRESERVE = 0xffff;

const uint reversedELFMagicConstant = 0x464C457F;

const uint symbolSizeBits = 16;

const uint instructionSize = 4;