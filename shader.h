#pragma once

#include <string>

class Shader {
public:
  explicit Shader(const std::string &vertex_path, const std::string &fragment_path);
  void Use() const;
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setVec3f(const std::string &name, float x, float y, float z) const;
  void setVec4f(const std::string &name, float x, float y, float z, float w) const;
  void setMat4f(const std::string &name, const float *p) const;

private:
  unsigned int program_;
};
