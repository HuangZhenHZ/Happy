#include "deps/glad.h"
#include "shader.h"
#include "io.h"

namespace {
template <GLenum shader_type>
GLuint GetShader(const char* filename) {
  std::vector<unsigned char> code = ReadFileContents(filename);
  if (code.empty()) {
    puts("ERROR: Empty shader file");
    return 0;
  }
  code.push_back(0);
  const char*const code_ptr = reinterpret_cast<char*>(code.data());
  GLuint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, &code_ptr, nullptr);
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(shader, 512, NULL, info_log);
    puts("ERROR: Shader compile error");
    puts(info_log);
  }
  return shader;
}
}  // namespace

Shader::Shader(const char* vertex_path, const char* fragment_path) {
  GLuint vertex_shader = GetShader<GL_VERTEX_SHADER>(vertex_path);
  GLuint fragment_shader = GetShader<GL_FRAGMENT_SHADER>(fragment_path);
  program_ = glCreateProgram();
  glAttachShader(program_, vertex_shader);
  glAttachShader(program_, fragment_shader);
  glLinkProgram(program_);
  int success;
  glGetProgramiv(program_, GL_LINK_STATUS, &success);
  if(!success) {
    char info_log[512];
    glGetProgramInfoLog(program_, 512, NULL, info_log);
    puts("ERROR: Link failed");
    puts(info_log);
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void Shader::Use() const {
  glUseProgram(program_);
}

void Shader::setBool(const char* name, bool value) const {
  glUniform1i(glGetUniformLocation(program_, name), (int)value);
}
void Shader::setInt(const char* name, int value) const {
  glUniform1i(glGetUniformLocation(program_, name), value);
}
void Shader::setFloat(const char* name, float value) const {
  glUniform1f(glGetUniformLocation(program_, name), value);
}
void Shader::setVec3f(const char* name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(program_, name), x, y, z);
}
void Shader::setVec4f(const char* name, float x, float y, float z, float w) const {
  glUniform4f(glGetUniformLocation(program_, name), x, y, z, w);
}
void Shader::setMat4f(const char* name, const float *p) const {
  glUniformMatrix4fv(glGetUniformLocation(program_, name), 1, GL_FALSE, p);
}
