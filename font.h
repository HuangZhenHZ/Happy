#pragma once

#include <memory>
#include <vector>

class Font {
public:
  Font() = default;
  virtual ~Font() = default;
  virtual void Draw(const std::vector<int>& codes, double x, double y) const = 0;
};

std::unique_ptr<Font> GetFont();
