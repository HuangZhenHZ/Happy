#include <fstream>
#include <iostream>
#include <sstream>
#include "deps/glad.h"
#include "shader.h"

namespace {
GLuint GetVertexShader(const std::string &vertex_path) {
  std::ifstream vertex_file(vertex_path);
  std::stringstream vertex_stream;
  vertex_stream << vertex_file.rdbuf();
  std::string vertex_code = vertex_stream.str();
  const char *vertex_code_ptr = vertex_code.c_str();
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_code_ptr, nullptr);
  glCompileShader(vertex_shader);
  int success;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
  }
  return vertex_shader;
}

GLuint GetFragmentShader(const std::string &fragment_path) {
  std::ifstream fragment_file(fragment_path);
  std::stringstream fragment_stream;
  fragment_stream << fragment_file.rdbuf();
  std::string fragment_code = fragment_stream.str();
  const char *fragment_code_ptr = fragment_code.c_str();
  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_code_ptr, nullptr);
  glCompileShader(fragment_shader);
  int success;
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info_log[512];
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;
  }
  return fragment_shader;
}
}  // namespace

Shader::Shader(const std::string &vertex_path, const std::string &fragment_path) {
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
    std::cout << "Link failed\n" << info_log << std::endl;
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void Shader::Use() const {
  glUseProgram(program_);
}

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(program_, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(program_, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(program_, name.c_str()), value);
}
void Shader::setVec3f(const std::string &name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(program_, name.c_str()), x, y, z);
}
void Shader::setVec4f(const std::string &name, float x, float y, float z, float w) const {
  glUniform4f(glGetUniformLocation(program_, name.c_str()), x, y, z, w);
}
void Shader::setMat4f(const std::string &name, const float *p) const {
  glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, p);
}
