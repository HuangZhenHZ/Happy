#include "draw.h"

#include "deps/glad.h"

#include <cassert>

ViewPort::Size ViewPort::size_;

void ViewPort::SetViewPort(int x, int y, int width, int height) {
  glViewport(x, y, width, height);
  size_.width = width;
  size_.height = height;
}

namespace {
GLenum ChannelsToFormat(int channels) {
  if (channels == 1) {
    return GL_RED;
  } else if (channels == 3) {
    return GL_RGB;
  } else if (channels == 4) {
    return GL_RGBA;
  } else {
    assert(false);
    return 0;
  }
}
};

Texture::Texture(int width, int height, int channels, const unsigned char *pixels) {
  GLenum format = ChannelsToFormat(channels);
  assert(format);
  glGenTextures(1, &id_);
  printf("gen texture %d\n", id_);
  glBindTexture(GL_TEXTURE_2D, id_);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::Delete() {
  printf("delete texture %d\n", id_);
  assert(id_ == 0 || glIsTexture(id_));
  glDeleteTextures(1, &id_);
  id_ = 0;
}

void Texture::Use() const {
  glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::SubImage(int x, int y, int width, int height, int channels, const unsigned char *pixels) {
  glBindTexture(GL_TEXTURE_2D, id_);
  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, ChannelsToFormat(channels), GL_UNSIGNED_BYTE, pixels);
}

namespace {
GLuint VAO1;
GLuint VBO1;
GLuint EBO1;

GLuint VAO2;
GLuint VBO2;
GLuint EBO2;

GLuint v3_rgb_vao;
GLuint v3_rgb_vbo;
GLuint v3_rgb_ebo;

GLuint v2_rgb_vao;
GLuint v2_rgb_vbo;
}  // namespace

void InitVAOVBO() {
  glGenVertexArrays(1, &VAO2);
  glBindVertexArray(VAO2);
  glGenBuffers(1, &VBO2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO2);
  glGenBuffers(1, &EBO2);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices3UvNormal::Vertex), (void*)offsetof(Vertices3UvNormal::Vertex, pos));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices3UvNormal::Vertex), (void*)offsetof(Vertices3UvNormal::Vertex, uv));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices3UvNormal::Vertex), (void*)offsetof(Vertices3UvNormal::Vertex, normal));
  glEnableVertexAttribArray(2);

  glGenVertexArrays(1, &VAO1);
  glBindVertexArray(VAO1);
  glGenBuffers(1, &VBO1);
  glBindBuffer(GL_ARRAY_BUFFER, VBO1);
  glGenBuffers(1, &EBO1);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices2UvRgba::Vertex), (void*)offsetof(Vertices2UvRgba::Vertex, x));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices2UvRgba::Vertex), (void*)offsetof(Vertices2UvRgba::Vertex, u));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertices2UvRgba::Vertex), (void*)offsetof(Vertices2UvRgba::Vertex, r));
  glEnableVertexAttribArray(2);
  // glVertexAttrib4f(2, 1.0, 1.0, 1.0, 1.0);

  glGenVertexArrays(1, &v3_rgb_vao);
  glBindVertexArray(v3_rgb_vao);
  glGenBuffers(1, &v3_rgb_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, v3_rgb_vbo);
  glGenBuffers(1, &v3_rgb_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v3_rgb_ebo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices3Rgb::Vertex), (void*)offsetof(Vertices3Rgb::Vertex, pos));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices3Rgb::Vertex), (void*)offsetof(Vertices3Rgb::Vertex, rgb));
  glEnableVertexAttribArray(1);

  glGenVertexArrays(1, &v2_rgb_vao);
  glBindVertexArray(v2_rgb_vao);
  glGenBuffers(1, &v2_rgb_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, v2_rgb_vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices2Rgb::Vertex), (void*)offsetof(Vertices2Rgb::Vertex, pos));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices2Rgb::Vertex), (void*)offsetof(Vertices2Rgb::Vertex, r));
  glEnableVertexAttribArray(1);
}

void Vertices2UvRgba::Draw() const {
  glBindVertexArray(VAO1);
  glBindBuffer(GL_ARRAY_BUFFER, VBO1);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STREAM_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(indices_[0]), indices_.data(), GL_STREAM_DRAW);
  // glDrawArrays(GL_TRIANGLES, 0, vertices_.size());
  glDrawElements(GL_TRIANGLES, indices_.size() * 3, GL_UNSIGNED_INT, 0);
}

void Vertices3UvNormal::AddToBuffer() const {
  glBindVertexArray(VAO2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO2);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STREAM_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(indices_[0]), indices_.data(), GL_STREAM_DRAW);
}

void Vertices3UvNormal::DrawCall() const {
  glBindVertexArray(VAO2);
  glDrawElements(GL_TRIANGLES, indices_.size() * 3, GL_UNSIGNED_INT, 0);
}

void Vertices3Rgb::AddToBuffer() const {
  glBindVertexArray(v3_rgb_vao);
  glBindBuffer(GL_ARRAY_BUFFER, v3_rgb_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STREAM_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(indices_[0]), indices_.data(), GL_STREAM_DRAW);
}

void Vertices3Rgb::DrawCall() const {
  glBindVertexArray(v3_rgb_vao);
  glDrawElements(GL_TRIANGLES, indices_.size() * 3, GL_UNSIGNED_INT, 0);
}

void Vertices2Rgb::AddToBuffer() const {
  glBindVertexArray(v2_rgb_vao);
  glBindBuffer(GL_ARRAY_BUFFER, v2_rgb_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STREAM_DRAW);
}

void Vertices2Rgb::DrawCall() const {
  glBindVertexArray(v2_rgb_vao);
  glDrawArrays(GL_POINTS, 0, vertices_.size());
}
