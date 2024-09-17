#include "font.h"

#include "deps/stb_truetype.h"
#include "draw.h"
#include "io.h"
#include "deps/glad.h"

#include <vector>
#include <cmath>
#include <chrono>
#include <memory>

namespace {
constexpr int texsz = 8192;
constexpr int cjkstart = 0x4e00;
constexpr int cjkend = 0x9FAF;

double font_x = 100;
double font_y = 500;

void ProcessFontChar(Vertices* vertices, const stbtt_packedchar& c, double scale) {
  // printf("c %f %f %f %f %f\n", c.xoff, c.yoff, c.xoff2, c.yoff2, c.xadvance);
  vertices->AddRect(font_x + c.xoff * scale, font_y + c.yoff * scale, font_x + c.xoff2 * scale, font_y + c.yoff2 * scale,
                    double(c.x0) / texsz, double(c.y0) / texsz, double(c.x1) / texsz, double(c.y1) / texsz);
  font_x += std::ceil(c.xadvance * scale);
}
}  // namespace

class FontStb : public Font {
  stbtt_packedchar packed_chars1_[96];
  stbtt_packedchar packed_chars2_[cjkend - cjkstart];
  std::unique_ptr<Texture> tex_;
public:
  explicit FontStb(const char* filename) {
    std::vector<unsigned char> ttf_file = ReadFileContents(filename);
    std::vector<unsigned char> temp_bitmap(texsz * texsz);

    stbtt_pack_context spc = {};
    stbtt_PackBegin(&spc, temp_bitmap.data(), texsz, texsz, 0 /* stride */, 1 /* padding */, NULL);
    stbtt_PackFontRange(&spc, ttf_file.data(), 0, 36.0, 32, 96, packed_chars1_);
    stbtt_PackFontRange(&spc, ttf_file.data(), 0, 36.0, cjkstart, cjkend - cjkstart, packed_chars2_);
    stbtt_PackEnd(&spc);

/*
    std::vector<unsigned char> rgba32map(texsz * texsz * 4);
    for (int i = 0; i < texsz * texsz; ++i) {
      rgba32map[i << 2 | 0] = 0xFF;
      rgba32map[i << 2 | 1] = 0xFF;
      rgba32map[i << 2 | 2] = 0xFF;
      rgba32map[i << 2 | 3] = temp_bitmap[i];
    }
    */

    tex_ = std::make_unique<Texture>(texsz, texsz, 1, temp_bitmap.data());
  }

  void Draw(const std::vector<int>& codes, double x, double y) override {
    font_x = x;
    font_y = y;
    Vertices vertices;
    for (const int &code : codes) {
      if (code >= 32 && code < 32 + 96) {
        ProcessFontChar(&vertices, packed_chars1_[code - 32], 1.0);
      } else if (code >= cjkstart && code < cjkend) {
        ProcessFontChar(&vertices, packed_chars2_[code - cjkstart], 1.0);
      } else {
        puts("Invalid char");
        return;
      }
    }
    tex_->Use();
    vertices.Draw();
  }
};

constexpr int texszv2 = 4096;

class FontV2 : public Font {
  std::vector<unsigned char> filedata_;
  std::unique_ptr<Texture> tex_;
  stbtt_fontinfo fontinfo_;
  double scale_;
public:
  explicit FontV2(const char *filename)
    : filedata_(ReadFileContents(filename)),
      tex_(std::make_unique<Texture>(texszv2, texszv2, 1, nullptr)) {
    stbtt_InitFont(&fontinfo_, filedata_.data(), 0);
    scale_ = stbtt_ScaleForPixelHeight(&fontinfo_, 72.0);
  }

  void Draw(const std::vector<int>& codes, double x, double y) override {
    current_tex_x_ = 100;
    current_tex_y_ = 100;
    font_x = x;
    font_y = y;
    Vertices vertices;
    for (const int code : codes) {
      MyPackData my_pack_data = GetData(code);
      double scale = 1.0;
      vertices.AddRect(font_x + my_pack_data.xoff * scale, font_y + my_pack_data.yoff * scale,
                       font_x + my_pack_data.xoff2 * scale, font_y + my_pack_data.yoff2 * scale,
                       double(my_pack_data.x0) / texszv2, double(my_pack_data.y0) / texszv2,
                       double(my_pack_data.x1) / texszv2, double(my_pack_data.y1) / texszv2);
      font_x += std::ceil(my_pack_data.xadvance * scale);
    }
    vertices.AddRect(0, 0, 500, 500, 0, 0, 0.5, 0.5);
    tex_->Use();
    vertices.Draw();
  }

private:
  int current_tex_x_ = 0, current_tex_y_ = 0;
  struct MyPackData {
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0; // coordinates of bbox in bitmap
    float xoff = 0, yoff = 0, xadvance = 0;
    float xoff2 = 0, yoff2 = 0;
  };
  MyPackData GetData(int codepoint) {
    int glyph = stbtt_FindGlyphIndex(&fontinfo_, codepoint);
    if (glyph == 0) {
      return MyPackData{};
    }
    int width = 0, height = 0, xoff = 0, yoff = 0;
    unsigned char* map = stbtt_GetGlyphBitmapSubpixel(&fontinfo_, scale_, scale_, 0.0, 0.0, glyph, &width, &height, &xoff, &yoff);

/*
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
         putchar(" .:ioVM@"[map[y * width + x]>>5]);
      }
      putchar('\n');
    }
    */

    int extended_width = width;
    int extended_height = height;
    extended_width += (4 - extended_width % 4) % 4;
    // extended_height += (4 - extended_height % 4) % 4;
    std::vector<unsigned char> extended_map(extended_width * extended_height);

    for (int y = 0; y < height; ++y) {
      memcpy(&extended_map[y * extended_width], map + y * width, width);
    }

    tex_->SubImage(current_tex_x_, current_tex_y_, extended_width, extended_height, 1, extended_map.data());

    MyPackData my_pack_data;
    my_pack_data.x0 = current_tex_x_;
    my_pack_data.y0 = current_tex_y_;
    my_pack_data.x1 = my_pack_data.x0 + width;
    my_pack_data.y1 = my_pack_data.y0 + height;
    my_pack_data.xoff = xoff;
    my_pack_data.yoff = yoff;
    my_pack_data.xoff2 = xoff + width;
    my_pack_data.yoff2 = yoff + height;
    int advance, lsb;
    stbtt_GetGlyphHMetrics(&fontinfo_, glyph, &advance, &lsb);

    my_pack_data.xadvance = advance * scale_;
    free(map);
    current_tex_x_ += extended_width + 1;
    return my_pack_data;
  }
};

std::unique_ptr<Font> GetFont() {
  return std::make_unique<FontV2>("SourceHanSansSC-Normal.otf");
  // return std::make_unique<FontV2>("wqy-microhei.ttc");
}
