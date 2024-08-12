#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "image.h"
#include "deps/stb_truetype.h"

bool DecodeUTF8(const std::string &utf8, std::vector<int> *result) {
  for (size_t i = 0; i < utf8.size(); ) {
    uint8_t c = utf8[i];
    // 1 byte
    if (c >> 7 == 0) {
      result->push_back(c);
      ++i;
    // 2 bytes
    } else if (c >> 5 == 6) {
      if (i + 1 >= utf8.size()) {
        return false;
      }
      uint8_t c1 = utf8[i + 1];
      if (c1 >> 6 != 2) {
        return false;
      }
      result->push_back((c & 31) << 6 | (c1 & 63));
      i += 2;
    // 3 bytes
    } else if (c >> 4 == 14) {
      if (i + 2 >= utf8.size()) {
        return false;
      }
      uint8_t c1 = utf8[i + 1];
      if (c1 >> 6 != 2) {
        return false;
      }
      uint8_t c2 = utf8[i + 2];
      if (c2 >> 6 != 2) {
        return false;
      }
      result->push_back((c & 15) << 12 | (c1 & 63) << 6 | (c2 & 63));
      i += 3;
    // 4 bytes
    } else if (c >> 3 == 30) {
      if (i + 3 >= utf8.size()) {
        return false;
      }
      uint8_t c1 = utf8[i + 1];
      if (c1 >> 6 != 2) {
        return false;
      }
      uint8_t c2 = utf8[i + 2];
      if (c2 >> 6 != 2) {
        return false;
      }
      uint8_t c3 = utf8[i + 3];
      if (c3 >> 6 != 2) {
        return false;
      }
      result->push_back((c & 7) << 18 | (c1 & 63) << 12 | (c2 & 63) << 6 | (c3 & 63));
      i += 4;
    } else {
      return false;
    }
  }
  return true;
}

int window_width = 1920;
int window_height = 1200;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  printf("resize %d %d\n", width, height);
  window_width = width;
  window_height = height;
  glViewport(0, 0, width, height);
}

struct Vertex {
  float x = 0.0, y = 0.0;
  float u = 0.0, v = 0.0;
  float r = 1.0, g = 1.0, b = 1.0, a = 1.0;
};

std::vector<Vertex> vertices;

void AddRect(float screen_x1, float screen_y1, float screen_x2, float screen_y2, float tex_x1, float tex_y1, float tex_x2, float tex_y2) {
  if (window_width == 0 || window_height == 0) {
    return;
  }
  screen_x1 = (screen_x1 / window_width) * 2 - 1;
  screen_x2 = (screen_x2 / window_width) * 2 - 1;
  screen_y1 = 1 - (screen_y1 / window_height) * 2;
  screen_y2 = 1 - (screen_y2 / window_height) * 2;
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
  if (!glfwInit()) {
    std::cout << "Failed to init GLFW" << std::endl;
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  const char title[] = "MyGame";
  GLFWwindow* window = glfwCreateWindow(window_width, window_height, title, nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glewExperimental = GL_TRUE;
  glewInit();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  FramebufferSizeCallback(window, window_width, window_height);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

  Shader shader("shader.vs.glsl", "shader.fs.glsl");
  shader.Use();

  GetFont();
  printf("error: %d\n", glGetError());

  std::vector<int> result;
  bool ok = DecodeUTF8(u8"中文测试The quick brown fox jumps over a lazy dog ij", &result);
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

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glfwSwapBuffers(window);
    glfwPollEvents();

    frame_cnt++;
    if (frame_cnt % 1000 == 0) {
      printf("frame_cnt = %d\n", frame_cnt);
      double new_time = glfwGetTime();
      printf("fps = %lf\n", 1000.0 / (new_time - last_time));
      last_time = new_time;
    }
  }

  glfwTerminate();
  return 0;
}
