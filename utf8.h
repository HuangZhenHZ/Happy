#include <string>
#include <vector>

bool DecodeUTF8(const std::string &utf8, std::vector<int> *result) {
  for (size_t i = 0; i < utf8.size(); ) {
    uint8_t c = utf8[i];
    // 1 byte
    if (c >> 7 == 0) {
      result->push_back(c);
      ++i;
    // 2 bytes
    } else if (c >> 5 == 6) {
      if (i + 1 >= utf8.size()) {
        return false;
      }
      uint8_t c1 = utf8[i + 1];
      if (c1 >> 6 != 2) {
        return false;
      }
      result->push_back((c & 31) << 6 | (c1 & 63));
      i += 2;
    // 3 bytes
    } else if (c >> 4 == 14) {
      if (i + 2 >= utf8.size()) {
        return false;
      }
      uint8_t c1 = utf8[i + 1];
      if (c1 >> 6 != 2) {
        return false;
      }
      uint8_t c2 = utf8[i + 2];
      if (c2 >> 6 != 2) {
        return false;
      }
      result->push_back((c & 15) << 12 | (c1 & 63) << 6 | (c2 & 63));
      i += 3;
    // 4 bytes
    } else if (c >> 3 == 30) {
      if (i + 3 >= utf8.size()) {
        return false;
      }
      uint8_t c1 = utf8[i + 1];
      if (c1 >> 6 != 2) {
        return false;
      }
      uint8_t c2 = utf8[i + 2];
      if (c2 >> 6 != 2) {
        return false;
      }
      uint8_t c3 = utf8[i + 3];
      if (c3 >> 6 != 2) {
        return false;
      }
      result->push_back((c & 7) << 18 | (c1 & 63) << 12 | (c2 & 63) << 6 | (c3 & 63));
      i += 4;
    } else {
      return false;
    }
  }
  return true;
}
