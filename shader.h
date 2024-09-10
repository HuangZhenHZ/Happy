#pragma once

#include <string>

class Shader {
public:
  explicit Shader(const char* vertex_path, const char* fragment_path);
  void Use() const;
  void setBool(const char* name, bool value) const;
  void setInt(const char* name, int value) const;
  void setFloat(const char* name, float value) const;
  void setVec3f(const char* name, float x, float y, float z) const;
  void setVec4f(const char* name, float x, float y, float z, float w) const;
  void setMat4f(const char* name, const float *p) const;

private:
  unsigned int program_;
};
