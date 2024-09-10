#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>

#include <cstdio>
#include <vector>

std::vector<unsigned char> ReadFileContents(const char* filename) {
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

std::string GetFileContentString(const char* filename) {
  std::ifstream file(filename);
  std::stringstream stream;
  stream << file.rdbuf();
  return stream.str();
}

int main() {
  {
    auto tp1 = std::chrono::steady_clock::now();
    int s = 0;
    for (int i = 0; i < 100; ++i) {
      std::string file_content = GetFileContentString("deps/stb_image.h");
      s += file_content.length();
    }
    printf("%d\n", s);
    auto tp2 = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp1).count() << " microseconds" << std::endl;
  }
  {
    auto tp1 = std::chrono::steady_clock::now();
    int s = 0;
    for (int i = 0; i < 100; ++i) {
      std::vector<unsigned char> file_content = ReadFileContents("deps/stb_image.h");
      file_content.push_back(0);
      s += file_content.size();
    }
    printf("%d\n", s);
    auto tp2 = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp1).count() << " microseconds" << std::endl;
  }
  {
    auto tp1 = std::chrono::steady_clock::now();
    int s = 0;
    for (int i = 0; i < 100; ++i) {
      std::string file_content = GetFileContentString("deps/stb_image.h");
      s += file_content.length();
    }
    printf("%d\n", s);
    auto tp2 = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp1).count() << " microseconds" << std::endl;
  }
  return 0;
}
