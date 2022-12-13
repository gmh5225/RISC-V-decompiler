#include "ElfReader.h"

using namespace std;

int main(int argc, char** argv) {
    ifstream in;
    in.open(argv[1], std::ios::binary);
    FILE * out = fopen(argv[2], "w");
    ElfReader er(in, out);
    er.process();
    in.close();
    return 0;
}
