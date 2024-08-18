#include <bits/stdc++.h>
#include "deps/glad.h"
#include "window.h"
#include "shader.h"
#include "image.h"
#include "deps/stb_truetype.h"
#include "utf8.h"

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
stbtt_packedchar packed_chars1[96];
stbtt_packedchar packed_chars2[cjkend - cjkstart];

void GetFont() {
  std::vector<uint8_t> ttf_buffer(20 << 20);
  std::vector<uint8_t> temp_bitmap(texsz * texsz);

  FILE* file = fopen("SourceHanSansSC-Normal.otf", "rb");
  int sz = fread(ttf_buffer.data(), 1, 20 << 20, file);
  printf("sz = %d\n", sz);
  fclose(file);

  stbtt_pack_context spc = {};
  stbtt_PackBegin(&spc, temp_bitmap.data(), texsz, texsz, 0 /* stride */, 1 /* padding */, NULL);
  stbtt_PackFontRange(&spc, ttf_buffer.data(), 0, 36.0, 32, 96, packed_chars1);
  stbtt_PackFontRange(&spc, ttf_buffer.data(), 0, 36.0, cjkstart, cjkend - cjkstart, packed_chars2);
  stbtt_PackEnd(&spc);

  std::vector<uint32_t> rgba32map(texsz * texsz);
  for (int i = 0; i < texsz * texsz; ++i) {
    rgba32map[i] = temp_bitmap[i] << 24 | 0xFFFFFF;
  }

  GLuint ftex;
  glGenTextures(1, &ftex);
  glBindTexture(GL_TEXTURE_2D, ftex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texsz, texsz, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap.data());
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texsz, texsz, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba32map.data());
}

void ProcessFontChar(const stbtt_packedchar& c, double scale) {
  static double x = 100;
  static double y = 500;
  // printf("c %f %f %f %f %f\n", c.xoff, c.yoff, c.xoff2, c.yoff2, c.xadvance);
  AddRect(x + c.xoff * scale, y + c.yoff * scale, x + c.xoff2 * scale, y + c.yoff2 * scale,
          double(c.x0) / texsz, double(c.y0) / texsz, double(c.x1) / texsz, double(c.y1) / texsz);
  x += std::ceil(c.xadvance * scale);
}

int main() {
  if (!Window::InitWindow(1920, 1080, "MyGame")) {
    return -1;
  }
  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Shader shader("shader.vs.glsl", "shader.fs.glsl");
  shader.Use();

  GetFont();
  printf("error: %d\n", glGetError());

  std::vector<int> result;
  bool ok = DecodeUTF8(u8"1234567890中文测试The quick brown fox jumps over a lazy dog ij", &result);
  if (!ok) {
    printf("Decode utf8 failed\n");
    return 0;
  }
  for (const int &code : result) {
    if (code >= 32 && code < 32 + 96) {
      ProcessFontChar(packed_chars1[code - 32], 1.0);
    } else if (code >= cjkstart && code < cjkend) {
      ProcessFontChar(packed_chars2[code - cjkstart], 1.0);
    } else {
      printf("Invalid char\n");
      return 0;
    }
  }

/*
  printf("vertices.size() = %llu\n", vertices.size());
  for (const Vertex& vertex : vertices) {
    printf("%lf %lf %lf %lf\n", vertex.x, vertex.y, vertex.u, vertex.v);
  }
  */

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, x));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, u));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, r));
  glEnableVertexAttribArray(2);

/*
  {
    GLuint texture0;
    glGenTextures(1, &texture0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // stbi_set_flip_vertically_on_load(true);
    Image image("awesomeface.png");
    Image image2("a.png");
    if (image.data()) {
      printf("width = %d, height = %d\n, channels = %d\n", image.width(), image.height(), image.channels());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
      glTexSubImage2D(GL_TEXTURE_2D, 0, 200, 20, image2.width(), image2.height(), GL_RGBA, GL_UNSIGNED_BYTE, image2.data());
      // glGenerateMipmap(GL_TEXTURE_2D);
      std::cout << "Load texture success" << std::endl;
    } else {
      std::cout << "Failed to load texture" << std::endl;
    }
  }
  */

  int frame_cnt = 0;
  double last_time = 0;

  while (!Window::WindowShouldClose()) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    Window::SwapScreenBuffer();
    Window::PollInputEvents();

    frame_cnt++;
    if (frame_cnt % 100 == 0) {
      printf("frame_cnt = %d\n", frame_cnt);
      double new_time = Window::GetTime();
      printf("fps = %lf\n", 1000.0 / (new_time - last_time));
      last_time = new_time;
    }
  }

  Window::CloseWindow();
  return 0;
}
