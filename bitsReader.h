#pragma once

#include <string>
#include "constants.h"

template<uint size1, uint size2>
inline static std::bitset<size1 + size2> concat(const std::bitset<size1> &b1, const std::bitset<size2> &b2) {
    std::string s1 = b1.to_string();
    std::string s2 = b2.to_string();
    return std::bitset<size1 + size2>(s1 + s2);
}

template<uint size1, uint size2, uint size3, uint size4>
static std::bitset<size1 + size2 + size3 + size4> inline
concat(const std::bitset<size1> &b1, const std::bitset<size2> &b2, const std::bitset<size3> &b3,
       const std::bitset<size4> &b4) {
    return std::bitset<size1 + size2 + size3 + size4>(
            concat<size1 + size2, size3 + size4>(concat<size1, size2>(b1, b2), concat<size3, size4>(b3, b4)));
}

template<uint from, uint to, uint srcSize = 32>
inline std::bitset<from - to + 1> getBits(std::bitset<srcSize> bits) {
    return std::bitset<from - to + 1>(
            bits.to_string().substr(srcSize - 1 - from, from - to + 1));
}

template<uint size>
inline int32_t convert2signed(std::bitset<size> num) {
    int32_t result = num.to_ulong();
    int pow = 1;
    for (int i = 0; i < size; i++) pow *= 2;
    if (num[size - 1]) result -= pow;
    return result;
}

class BitsReader {
public:
    BitsReader(Bytes &&bytes) : source(bytes) {}

    inline uint getByte(uint offset) const { return source[offset].to_ulong(); }

    inline uint getByte2(uint offset) const {
        return concat<8, 8>(source[offset + 1], source[offset]).to_ulong();
    }

    inline uint getByte4(uint offset) const {
        return concat<8, 8, 8, 8>(source[offset + 3], source[offset + 2], source[offset + 1],
                                  source[offset]).to_ulong();
    }

    uint operator[](uint offset) const { return getByte(offset); }

private:
    Bytes source = {};
};

