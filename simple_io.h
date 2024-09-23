#pragma once

#include <cstdio>
#include <vector>

inline std::vector<unsigned char> ReadFileContents(const char* filename) {
  FILE* file = fopen(filename, "rb");
  if (file == nullptr) {
    return {};
  }
  fseek(file, 0, SEEK_END);
  int filesize = ftell(file);
  fseek(file, 0, SEEK_SET);
  std::vector<unsigned char> buffer;
  buffer.reserve(filesize + 1);
  buffer.resize(filesize);
  fread(buffer.data(), filesize, 1, file);
  fclose(file);
  return buffer;
}
