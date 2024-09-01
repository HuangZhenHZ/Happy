#include "font.h"

#include "deps/stb_truetype.h"
#include "window.h"
#include "texture.h"

#include <vector>
#include <cstdio>
#include <optional>
#include <cmath>
#include <memory>

namespace {
struct Vertex {
  float x = 0.0, y = 0.0;
  float u = 0.0, v = 0.0;
  float r = 1.0, g = 1.0, b = 1.0, a = 1.0;
};

std::vector<Vertex> vertices;

void AddRect(float screen_x1, float screen_y1, float screen_x2, float screen_y2, float tex_x1, float tex_y1, float tex_x2, float tex_y2) {
  if (Window::width() == 0 || Window::height() == 0) {
    return;
  }
  screen_x1 = (screen_x1 / Window::width()) * 2 - 1;
  screen_x2 = (screen_x2 / Window::width()) * 2 - 1;
  screen_y1 = 1 - (screen_y1 / Window::height()) * 2;
  screen_y2 = 1 - (screen_y2 / Window::height()) * 2;
  Vertex v0{.x = screen_x1, .y = screen_y1, .u = tex_x1, .v = tex_y1};
  Vertex v1{.x = screen_x2, .y = screen_y1, .u = tex_x2, .v = tex_y1};
  Vertex v2{.x = screen_x2, .y = screen_y2, .u = tex_x2, .v = tex_y2};
  Vertex v3{.x = screen_x1, .y = screen_y2, .u = tex_x1, .v = tex_y2};
  vertices.push_back(v0);
  vertices.push_back(v1);
  vertices.push_back(v2);
  vertices.push_back(v2);
  vertices.push_back(v3);
  vertices.push_back(v0);
}

constexpr int texsz = 8192;
constexpr int cjkstart = 0x4e00;
constexpr int cjkend = 0x9FAF;

double font_x = 100;
double font_y = 500;

void ProcessFontChar(const stbtt_packedchar& c, double scale) {
  // printf("c %f %f %f %f %f\n", c.xoff, c.yoff, c.xoff2, c.yoff2, c.xadvance);
  AddRect(font_x + c.xoff * scale, font_y + c.yoff * scale, font_x + c.xoff2 * scale, font_y + c.yoff2 * scale,
          double(c.x0) / texsz, double(c.y0) / texsz, double(c.x1) / texsz, double(c.y1) / texsz);
  font_x += std::ceil(c.xadvance * scale);
}
}  // namespace

class FontStb : public Font {
  stbtt_packedchar packed_chars1_[96];
  stbtt_packedchar packed_chars2_[cjkend - cjkstart];
  std::optional<Texture> tex_;
public:
  explicit FontStb(const char* filename) {
    std::vector<unsigned char> ttf_buffer(20 << 20);
    std::vector<unsigned char> temp_bitmap(texsz * texsz);

    FILE* file = fopen(filename, "rb");
    int sz = fread(ttf_buffer.data(), 1, 20 << 20, file);
    printf("sz = %d\n", sz);
    fclose(file);

    stbtt_pack_context spc = {};
    stbtt_PackBegin(&spc, temp_bitmap.data(), texsz, texsz, 0 /* stride */, 1 /* padding */, NULL);
    stbtt_PackFontRange(&spc, ttf_buffer.data(), 0, 36.0, 32, 96, packed_chars1_);
    stbtt_PackFontRange(&spc, ttf_buffer.data(), 0, 36.0, cjkstart, cjkend - cjkstart, packed_chars2_);
    stbtt_PackEnd(&spc);

  /*
    std::vector<uint32_t> rgba32map(texsz * texsz);
    for (int i = 0; i < texsz * texsz; ++i) {
      rgba32map[i] = temp_bitmap[i] << 24 | 0xFFFFFF;
    }
    */

    tex_ = Texture(texsz, texsz, GL_RED, temp_bitmap.data());
  }

  void Draw(const std::vector<int>& codes, double x, double y) const override {
    font_x = x;
    font_y = y;
    vertices.clear();
    for (const int &code : codes) {
      if (code >= 32 && code < 32 + 96) {
        ProcessFontChar(packed_chars1_[code - 32], 1.0);
      } else if (code >= cjkstart && code < cjkend) {
        ProcessFontChar(packed_chars2_[code - cjkstart], 1.0);
      } else {
        printf("Invalid char\n");
        return;
      }
    }
    tex_->Use();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  }
};

std::unique_ptr<Font> GetFont() {
  return std::make_unique<FontStb>("SourceHanSansSC-Normal.otf");
}
