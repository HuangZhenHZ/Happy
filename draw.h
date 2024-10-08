#pragma once

#include <cstdio>
#include <vector>

void InitVAOVBO();

class ViewPort {
public:
  struct Size { int width = 0, height = 0; };
  static Size size() { return size_; }
  static void SetViewPort(int x, int y, int width, int height);
private:
  static Size size_;
};

template <class Obj>
class UniqueObj {
  Obj obj_;
public:
  template<typename... _Args>
  explicit UniqueObj(_Args&&... __args) : obj_(std::forward<_Args>(__args)...) {}
  ~UniqueObj() {
    obj_.Delete();
  }
  const Obj& Get() const {
    return obj_;
  }
  Obj* operator->() {
    return &obj_;
  }
  UniqueObj(const UniqueObj& u) = delete;
  UniqueObj(UniqueObj&& u) {
    obj_ = u.obj_;
    u.obj_ = Obj{};
  }
  UniqueObj& operator= (const UniqueObj& u) = delete;
  UniqueObj& operator= (UniqueObj&& u) {
    Obj temp = u.obj_;
    u.obj_ = Obj{};
    obj_.Delete();
    obj_ = temp;
    return *this;
  }
  void Reset() {
    obj_.Delete();
  }
};

class Texture {
  unsigned int id_ = 0;
  explicit Texture(int width, int height, int channels, const unsigned char *pixels);
  void Delete();
  friend class UniqueObj<Texture>;
public:
  Texture() = default;
  bool NotNull() const {
    return id_;
  }
  void Use() const;
  void SubImage(int x, int y, int width, int height, int channels, const unsigned char *pixels);
};

using UniqueTexture = UniqueObj<Texture>;

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
