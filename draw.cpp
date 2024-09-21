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

UniqueTexture::UniqueTexture(int width, int height, int channels, const unsigned char *pixels) {
  GLenum format = ChannelsToFormat(channels);
  assert(format);
  glGenTextures(1, &tex_.id_);
  glBindTexture(GL_TEXTURE_2D, tex_.id_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
}

UniqueTexture::~UniqueTexture() {
  assert(tex_.id_ == 0 || glIsTexture(tex_.id_));
  glDeleteTextures(1, &tex_.id_);
}

void UniqueTexture::Reset() {
  assert(tex_.id_ == 0 || glIsTexture(tex_.id_));
  glDeleteTextures(1, &tex_.id_);
  tex_.id_ = 0;
}

void Texture::Use() const {
  glBindTexture(GL_TEXTURE_2D, id_);
}

void Texture::SubImage(int x, int y, int width, int height, int channels, const unsigned char *pixels) {
  glBindTexture(GL_TEXTURE_2D, id_);
  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, ChannelsToFormat(channels), GL_UNSIGNED_BYTE, pixels);
}

// glTexSubImage2D(GL_TEXTURE_2D, 0, 200, 20, image2.width(), image2.height(), GL_RGBA, GL_UNSIGNED_BYTE, image2.data());

void Vertices::Draw() const {
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STREAM_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, vertices_.size());
}
