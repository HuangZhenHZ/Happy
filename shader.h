#pragma once

#include <string>
#include <unordered_map>

class Shader {
  unsigned int program_ = 0;
public:
  explicit Shader(const char* vertex_path, const char* fragment_path);
  explicit Shader(const char* geo_path, const char* vertex_path, const char* fragment_path);
  void Use() const;
  void setBool(const char* name, bool value) const;
  void setInt(const char* name, int value) const;
  void setFloat(const char* name, float value) const;
  void setVec3f(const char* name, float x, float y, float z) const;
  void setVec4f(const char* name, float x, float y, float z, float w) const;
  void setMat4f(const char* name, const float *p) const;
};

class ShaderManager {
  inline static std::unordered_map<std::string, Shader> shader_map_;
public:
  static Shader GetShader(const std::string& vertex_path, const std::string& fragment_path) {
    return shader_map_.try_emplace(vertex_path + "|" + fragment_path, ("shaders/" + vertex_path).c_str(), ("shaders/" + fragment_path).c_str()).first->second;
  }
};
