#pragma once

#include "vec.h"

#include <algorithm>
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
  bool Null() const {
    return !bool(id_);
  }
  void Use() const;
  void SubImage(int x, int y, int width, int height, int channels, const unsigned char *pixels);
  unsigned int id() const {
    return id_;
  }
};

using UniqueTexture = UniqueObj<Texture>;

struct TriIndex {
  unsigned int a, b, c;
  TriIndex(unsigned int a, unsigned int b, unsigned int c) : a(a), b(b), c(c) {}
};

class Vertices2UvRgba {
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
    int sz = vertices_.size();
    vertices_.push_back(v0);
    vertices_.push_back(v1);
    vertices_.push_back(v2);
    // vertices_.push_back(v2);
    vertices_.push_back(v3);
    // vertices_.push_back(v0);
    indices_.emplace_back(sz + 0, sz + 1, sz + 2);
    indices_.emplace_back(sz + 2, sz + 3, sz + 0);
  }
  void Draw() const;
private:
  std::vector<Vertex> vertices_;
  std::vector<TriIndex> indices_;
};

struct UV {
  float u = 0.0, v = 0.0;
};

class Vertices3UvNormal {
public:
  struct Vertex {
    Vec3f pos;
    UV uv;
    Vec3f normal;
  };

  void AddRect3d(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    int sz = vertices_.size();
    vertices_.push_back(v0);
    vertices_.push_back(v1);
    vertices_.push_back(v2);
    vertices_.push_back(v3);
    indices_.emplace_back(sz + 0, sz + 1, sz + 2);
    indices_.emplace_back(sz + 2, sz + 3, sz + 0);
  }

  void AddBox(const Vec3f center, const Vec3f e1, const Vec3f e2, const Vec3f e3) {
    const Vec3f e1_normal = e1.Normalized();
    const Vec3f e2_normal = e2.Normalized();
    const Vec3f e3_normal = e3.Normalized();

    AddRect3d(Vertex{center + e3 - e1 - e2, {0.0f, 0.0f}, e3_normal},
              Vertex{center + e3 + e1 - e2, {1.0f, 0.0f}, e3_normal},
              Vertex{center + e3 + e1 + e2, {1.0f, 1.0f}, e3_normal},
              Vertex{center + e3 - e1 + e2, {0.0f, 1.0f}, e3_normal});

    AddRect3d(Vertex{center - e3 + e1 - e2, {0.0f, 0.0f}, -e3_normal},
              Vertex{center - e3 - e1 - e2, {1.0f, 0.0f}, -e3_normal},
              Vertex{center - e3 - e1 + e2, {1.0f, 1.0f}, -e3_normal},
              Vertex{center - e3 + e1 + e2, {0.0f, 1.0f}, -e3_normal});

    AddRect3d(Vertex{center + e1 - e2 - e3, {0.0f, 0.0f}, e1_normal},
              Vertex{center + e1 + e2 - e3, {1.0f, 0.0f}, e1_normal},
              Vertex{center + e1 + e2 + e3, {1.0f, 1.0f}, e1_normal},
              Vertex{center + e1 - e2 + e3, {0.0f, 1.0f}, e1_normal});

    AddRect3d(Vertex{center - e1 + e2 - e3, {0.0f, 0.0f}, -e1_normal},
              Vertex{center - e1 - e2 - e3, {1.0f, 0.0f}, -e1_normal},
              Vertex{center - e1 - e2 + e3, {1.0f, 1.0f}, -e1_normal},
              Vertex{center - e1 + e2 + e3, {0.0f, 1.0f}, -e1_normal});

    AddRect3d(Vertex{center + e2 + e1 - e3, {0.0f, 0.0f}, e2_normal},
              Vertex{center + e2 - e1 - e3, {1.0f, 0.0f}, e2_normal},
              Vertex{center + e2 - e1 + e3, {1.0f, 1.0f}, e2_normal},
              Vertex{center + e2 + e1 + e3, {0.0f, 1.0f}, e2_normal});

    AddRect3d(Vertex{center - e2 - e1 - e3, {0.0f, 0.0f}, -e2_normal},
              Vertex{center - e2 + e1 - e3, {1.0f, 0.0f}, -e2_normal},
              Vertex{center - e2 + e1 + e3, {1.0f, 1.0f}, -e2_normal},
              Vertex{center - e2 - e1 + e3, {0.0f, 1.0f}, -e2_normal});
  }

  void AddToBuffer() const;
  void DrawCall() const;

  void SortFromFarToNear(const Vec3f &pos) {
    // TODO: optimize and benchmark
    std::sort(indices_.begin(), indices_.end(),
        [&](const TriIndex &t1, const TriIndex &t2) {
          Vec3f p1 = (vertices_[t1.a].pos + vertices_[t1.b].pos + vertices_[t1.c].pos);
          Vec3f p2 = (vertices_[t2.a].pos + vertices_[t2.b].pos + vertices_[t2.c].pos);
          return (pos - p1).Length() > (pos - p2).Length();
        }
    );
  }

private:
  std::vector<Vertex> vertices_;
  std::vector<TriIndex> indices_;
};

struct RGB {
  float r = 0.0, g = 0.0, b = 0.0;
};

class Vertices3Rgb {
public:
  struct Vertex {
    Vec3f pos;
    RGB rgb;
  };

  void AddRect3d(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    int sz = vertices_.size();
    vertices_.push_back(v0);
    vertices_.push_back(v1);
    vertices_.push_back(v2);
    vertices_.push_back(v3);
    indices_.emplace_back(sz + 0, sz + 1, sz + 2);
    indices_.emplace_back(sz + 2, sz + 3, sz + 0);
  }

  void AddBox(const Vec3f center, const Vec3f e1, const Vec3f e2, const Vec3f e3, const RGB rgb) {
    vertices_.clear();
    indices_.clear();
    AddRect3d(Vertex{center + e3 - e1 - e2, rgb},
              Vertex{center + e3 + e1 - e2, rgb},
              Vertex{center + e3 + e1 + e2, rgb},
              Vertex{center + e3 - e1 + e2, rgb});

    AddRect3d(Vertex{center - e3 + e1 - e2, rgb},
              Vertex{center - e3 - e1 - e2, rgb},
              Vertex{center - e3 - e1 + e2, rgb},
              Vertex{center - e3 + e1 + e2, rgb});

    AddRect3d(Vertex{center + e1 - e2 - e3, rgb},
              Vertex{center + e1 + e2 - e3, rgb},
              Vertex{center + e1 + e2 + e3, rgb},
              Vertex{center + e1 - e2 + e3, rgb});

    AddRect3d(Vertex{center - e1 + e2 - e3, rgb},
              Vertex{center - e1 - e2 - e3, rgb},
              Vertex{center - e1 - e2 + e3, rgb},
              Vertex{center - e1 + e2 + e3, rgb});

    AddRect3d(Vertex{center + e2 + e1 - e3, rgb},
              Vertex{center + e2 - e1 - e3, rgb},
              Vertex{center + e2 - e1 + e3, rgb},
              Vertex{center + e2 + e1 + e3, rgb});

    AddRect3d(Vertex{center - e2 - e1 - e3, rgb},
              Vertex{center - e2 + e1 - e3, rgb},
              Vertex{center - e2 + e1 + e3, rgb},
              Vertex{center - e2 - e1 + e3, rgb});
  }

  void AddToBuffer() const;
  void DrawCall() const;

private:
  std::vector<Vertex> vertices_;
  std::vector<TriIndex> indices_;
};
