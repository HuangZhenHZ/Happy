#include "deps/glad.h"
#include "shader.h"
#include "io.h"

namespace {
GLuint GetVertexShader(const char* vertex_path) {
  std::vector<unsigned char> vertex_shader_code = ReadFileContents(vertex_path);
  vertex_shader_code.push_back(0);
  const char*const vertex_code_ptr = reinterpret_cast<char*>(vertex_shader_code.data());
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_code_ptr, nullptr);
  glCompileShader(vertex_shader);
  int success;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    puts("ERROR::SHADER::VERTEX::COMPILATION_FAILED");
    puts(info_log);
  }
  return vertex_shader;
}

GLuint GetFragmentShader(const char* fragment_path) {
  std::vector<unsigned char> fragment_shader_code = ReadFileContents(fragment_path);
  fragment_shader_code.push_back(0);
  const char*const fragment_code_ptr = reinterpret_cast<char*>(fragment_shader_code.data());
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_code_ptr, nullptr);
  glCompileShader(fragment_shader);
  int success;
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    puts("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");
    puts(info_log);
  }
  return fragment_shader;
}
}  // namespace

Shader::Shader(const char* vertex_path, const char* fragment_path) {
  GLuint vertex_shader = GetVertexShader(vertex_path);
  GLuint fragment_shader = GetFragmentShader(fragment_path);
  program_ = glCreateProgram();
  glAttachShader(program_, vertex_shader);
  glAttachShader(program_, fragment_shader);
  glLinkProgram(program_);
  int success;
  glGetProgramiv(program_, GL_LINK_STATUS, &success);
  if(!success) {
    char info_log[512];
    glGetProgramInfoLog(program_, 512, NULL, info_log);
    puts("Link failed");
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
