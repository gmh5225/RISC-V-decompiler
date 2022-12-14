#include "ElfReader.h"

int main(int argc, char **argv) {
    if(argc < 2) {
        std::cout << "Error! Wrong amount of arguments.";
        return 0;
    }

    std::ifstream in(argv[1], std::ios::binary);
    if(!in.is_open()) {
        std::cout << "Error! Impossible to open input file.";
        return 0;
    }

    FILE *out = fopen(argv[2], "w");
    if(out == nullptr) {
        std::cout << "Error! Impossible to open output file.";
        return 0;
    }

    ElfReader er(in, out);
    er.process();
    in.close();
    fclose(out);
    return 0;
}
