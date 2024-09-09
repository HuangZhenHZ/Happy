#pragma once

#include <vector>

class ViewPort {
public:
  struct Size { int width = 0, height = 0; };
  static Size size() { return size_; }
  static void SetViewPort(int x, int y, int width, int height);
private:
  static Size size_;
};

class Texture {
  unsigned int tex_ = 0;
public:
  explicit Texture(int width, int height, int channels, const unsigned char *pixels);
  ~Texture();
  Texture(const Texture& obj) = delete;
  Texture(Texture&& obj) = delete;
  Texture& operator= (const Texture& obj) = delete;
  Texture& operator= (Texture&& obj) = delete;

  void Use() const;
};

class Vertices {
public:
  struct Vertex {
    float x = 0.0, y = 0.0;
    float u = 0.0, v = 0.0;
    float r = 1.0, g = 1.0, b = 1.0, a = 1.0;
  };
  void AddRect(float screen_x1,
               float screen_y1,
               float screen_x2,
               float screen_y2,
               float tex_x1,
               float tex_y1,
               float tex_x2,
               float tex_y2) {
    ViewPort::Size viewport_size = ViewPort::size();
    if (viewport_size.width == 0 || viewport_size.height == 0) {
      return;
    }
    screen_x1 = (screen_x1 / viewport_size.width) * 2 - 1;
    screen_x2 = (screen_x2 / viewport_size.width) * 2 - 1;
    screen_y1 = 1 - (screen_y1 / viewport_size.height) * 2;
    screen_y2 = 1 - (screen_y2 / viewport_size.height) * 2;
    Vertex v0{.x = screen_x1, .y = screen_y1, .u = tex_x1, .v = tex_y1};
    Vertex v1{.x = screen_x2, .y = screen_y1, .u = tex_x2, .v = tex_y1};
    Vertex v2{.x = screen_x2, .y = screen_y2, .u = tex_x2, .v = tex_y2};
    Vertex v3{.x = screen_x1, .y = screen_y2, .u = tex_x1, .v = tex_y2};
    vertices_.push_back(v0);
    vertices_.push_back(v1);
    vertices_.push_back(v2);
    vertices_.push_back(v2);
    vertices_.push_back(v3);
    vertices_.push_back(v0);
  }
  void Draw() const;
private:
  std::vector<Vertex> vertices_;
};
