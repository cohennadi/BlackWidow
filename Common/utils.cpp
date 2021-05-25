#include "utils.h"

#include <cstring>
#include <fstream>
#include <iterator>


std::vector<char> readFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open() || !file.good()) return {};

    file.unsetf(std::ios::skipws);
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> vec;
    vec.reserve(fileSize);

    vec.insert(vec.begin(),
               std::istream_iterator<char>(file),
               std::istream_iterator<char>());

    return vec;
}
