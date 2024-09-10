#include "font.h"

#include "deps/stb_truetype.h"
#include "window.h"
#include "draw.h"
#include "io.h"

#include <vector>
#include <cstdio>
#include <optional>
#include <cmath>
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
    std::vector<uint32_t> rgba32map(texsz * texsz);
    for (int i = 0; i < texsz * texsz; ++i) {
      rgba32map[i] = temp_bitmap[i] << 24 | 0xFFFFFF;
    }
    */

    tex_ = std::make_unique<Texture>(texsz, texsz, 1, temp_bitmap.data());
  }

  void Draw(const std::vector<int>& codes, double x, double y) const override {
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

std::unique_ptr<Font> GetFont() {
  return std::make_unique<FontStb>("SourceHanSansSC-Normal.otf");
}
