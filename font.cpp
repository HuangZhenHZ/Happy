#include "font.h"

#include "deps/stb_truetype.h"
#include "draw.h"
#include "io.h"
#include "deps/glad.h"
#include "shader.h"

#include <vector>
#include <cmath>
#include <chrono>
#include <memory>
#include <unordered_map>

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
  font_x += c.xadvance * scale;
}
}  // namespace

class FontStb : public Font {
  stbtt_packedchar packed_chars1_[96];
  stbtt_packedchar packed_chars2_[cjkend - cjkstart];
  UniqueTexture tex_;
public:
  explicit FontStb(const char* filename) {
    std::vector<unsigned char> ttf_file = ReadFileContents(filename);
    std::vector<unsigned char> temp_bitmap(texsz * texsz);

    stbtt_pack_context spc = {};
    stbtt_PackBegin(&spc, temp_bitmap.data(), texsz, texsz, 0 /* stride */, 1 /* padding */, NULL);
    stbtt_PackSetOversampling(&spc, 2, 1);
    stbtt_PackFontRange(&spc, ttf_file.data(), 0, 36.0, 32, 96, packed_chars1_);
    stbtt_PackFontRange(&spc, ttf_file.data(), 0, 36.0, cjkstart, cjkend - cjkstart, packed_chars2_);
    stbtt_PackEnd(&spc);

    // std::vector<unsigned char> rgba32map(texsz * texsz * 4);
    // for (int i = 0; i < texsz * texsz; ++i) {
    //   rgba32map[i << 2 | 0] = 0xFF;
    //   rgba32map[i << 2 | 1] = 0xFF;
    //   rgba32map[i << 2 | 2] = 0xFF;
    //   rgba32map[i << 2 | 3] = temp_bitmap[i];
    // }

    tex_ = UniqueTexture(texsz, texsz, 1, temp_bitmap.data());
    // tex_ = std::make_unique<Texture>(texsz, texsz, 4, rgba32map.data());
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
        // printf("Invalid char: code = %d\n", code);
        // return;
      }
    }
    vertices.AddRect(0, 0, 256, 256, 0, 0, 256.0 / texsz, 256.0 / texsz);
    tex_->Use();
    vertices.Draw();
  }
};

class AutoTimer {
  const std::chrono::time_point<std::chrono::steady_clock> tp_;
public:
  AutoTimer() : tp_(std::chrono::steady_clock::now()) {}
  ~AutoTimer() {
    auto tp2 = std::chrono::steady_clock::now();
    printf("time = %lld\n", std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp_).count());
  }
};

class FontV2 : public Font {
  static constexpr int texszv2 = 1024;
  std::vector<unsigned char> filedata_;
  UniqueTexture tex_;
  stbtt_fontinfo fontinfo_;
  double scale_;
  Shader shader_;
public:
  explicit FontV2(const char *filename)
    : filedata_(ReadFileContents(filename)),
      tex_(UniqueTexture(texszv2, texszv2, 1, nullptr)),
      shader_(ShaderManager::GetShader("shader.vs.glsl", "shader.font.fs.glsl")) {
    stbtt_InitFont(&fontinfo_, filedata_.data(), stbtt_GetFontOffsetForIndex(filedata_.data(), 0));
    scale_ = stbtt_ScaleForPixelHeight(&fontinfo_, 36.0);
  }

  void Draw(const std::vector<int>& codes, double x, double y) override {
    if (current_tex_y_ > 800) {
      current_tex_x_ = 0;
      current_tex_y_ = 0;
      current_tex_y_max_ = 0;
      data_map_.clear();
    }
    font_x = x;
    font_y = y;
    Vertices vertices;
    for (int i = 0; i < (int)codes.size(); ++i) {
      int code = codes[i];
      if (code == '\n') {
        font_x = x;
        font_y += 36;
      }
      MyPackData my_pack_data = GetData(code);
      double scale = 1.0;
      vertices.AddRect(font_x + my_pack_data.xoff * scale, font_y + my_pack_data.yoff * scale,
                       font_x + my_pack_data.xoff2 * scale, font_y + my_pack_data.yoff2 * scale,
                       double(my_pack_data.x0) / texszv2, double(my_pack_data.y0) / texszv2,
                       double(my_pack_data.x1) / texszv2, double(my_pack_data.y1) / texszv2);
      double advance = my_pack_data.xadvance;
      // if (i + 1 < (int)codes.size()) {
      //   double kern = stbtt_GetCodepointKernAdvance(&fontinfo_, code, codes[i + 1]);
      //   advance += scale_ * kern;
      //   // if (kern != 0) {
      //   //   printf("i = %d, kern = %lf, (%d, %d)\n", i, kern, code, codes[i + 1]);
      //   // }
      // }
      // font_x += std::ceil(advance * scale);
      font_x += advance * scale;
    }
    vertices.AddRect(0, 0, 256, 256, 0, 0, 1, 1);
    shader_.Use();
    tex_->Use();
    vertices.Draw();
  }

private:
  int current_tex_x_ = 0, current_tex_y_ = 0, current_tex_y_max_ = 0;
  struct MyPackData {
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0; // coordinates of bbox in bitmap
    int xoff = 0, yoff = 0, xoff2 = 0, yoff2 = 0;
    float xadvance = 0;
  };
  std::unordered_map<int, MyPackData> data_map_;

  MyPackData GetData(int codepoint) {
    // AutoTimer autotimer{};
    if (auto it = data_map_.find(codepoint); it != data_map_.end()) {
      return it->second;
    }
    int glyph = stbtt_FindGlyphIndex(&fontinfo_, codepoint);
    if (glyph == 0) {
      return MyPackData{};
    }
    int width = 0, height = 0, xoff = 0, yoff = 0;
    unsigned char* map = stbtt_GetGlyphBitmapSubpixel(&fontinfo_, scale_, scale_, 0.0, 0.0, glyph, &width, &height, &xoff, &yoff);
    if (map == nullptr) {
      int advance, lsb;
      stbtt_GetGlyphHMetrics(&fontinfo_, glyph, &advance, &lsb);
      return data_map_[codepoint] = MyPackData{.xadvance = float(advance * scale_) };
    }

    // for (int y = 0; y < height; ++y) {
    //   for (int x = 0; x < width; ++x) {
    //      putchar(" .:ioVM@"[map[y * width + x]>>5]);
    //   }
    //   putchar('\n');
    // }

    int extended_width = width;
    int extended_height = height;
    extended_width += (4 - extended_width % 4) % 4;
    // extended_height += (4 - extended_height % 4) % 4;
    std::vector<unsigned char> extended_map(extended_width * extended_height);

    for (int y = 0; y < height; ++y) {
      memcpy(&extended_map[y * extended_width], map + y * width, width);
    }

    if (current_tex_x_ + extended_width > texszv2) {
      current_tex_x_ = 0;
      current_tex_y_ = current_tex_y_max_;
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
    current_tex_x_ += width + 1;
    current_tex_y_max_ = std::max(current_tex_y_max_, current_tex_y_ + height + 1);
    return data_map_[codepoint] = my_pack_data;
  }
};

std::unique_ptr<Font> GetFont() {
  // return std::make_unique<FontV2>("c:/windows/Fonts/msyh.ttc");
  return std::make_unique<FontV2>("NotoSansSC-Regular.otf");
}
