#include "deps/glad.h"
#include "window.h"
#include "shader.h"
#include "image.h"
#include "deps/stb_truetype.h"
#include "utf8.h"
#include "draw.h"
#include "font.h"

#include <iostream>
#include <cmath>
#include <memory>

#ifdef _glfw3_h_
#error Should not include <GLFW/glfw3.h>
#endif

std::unique_ptr<Texture> GetTextureFromFile(const char* filename) {
  Image image("awesomeface.png");
  if (image.data()) {
    printf("width = %d, height = %d, channels = %d\n", image.width(), image.height(), image.channels());
    return std::make_unique<Texture>(image.width(), image.height(), image.channels(), image.data());
    // std::cout << "Load texture success" << std::endl;
  }
  return nullptr;
}

int main() {
  if (!Window::InitWindow(1920, 1080, "MyGame")) {
    return -1;
  }
  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Shader shader("shader.vs.glsl", "shader.fs.glsl");
  shader.Use();

  std::unique_ptr<Texture> tex = GetTextureFromFile("awesomeface.png");
  if (tex == nullptr) {
    printf("Load texture error\n");
    printf("error: %d\n", glGetError());
  }

  std::unique_ptr<Font> font = GetFont();
  printf("error: %d\n", glGetError());

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertices::Vertex, x));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertices::Vertex, u));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertices::Vertex, r));
  glEnableVertexAttribArray(2);

  int frame_cnt = 0;
  double last_time = 0;

  while (!Window::WindowShouldClose()) {
    ViewPort::SetViewPort(0, 0, Window::width(), Window::height());
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double offset = Window::GetTime() * 50;

    std::vector<int> result;
    bool ok = DecodeUTF8(u8"1234567890中文测试The quick brown fox jumps over a lazy dog ij (1 + 2) * 3", &result);
    if (!ok) {
      printf("Decode utf8 failed\n");
      return 0;
    }
    font->Draw(result, 100 + offset, 500);

    tex->Use();
    Vertices vertices;
    vertices.AddRect(offset, offset, 300 + offset, 300 + offset, 0, 0, 1, 1);
    vertices.Draw();

    Window::SwapScreenBuffer();
    Window::PollInputEvents();

    frame_cnt++;
    if (frame_cnt % 100 == 0) {
      printf("frame_cnt = %d\n", frame_cnt);
      double new_time = Window::GetTime();
      printf("fps = %lf\n", 100.0 / (new_time - last_time));
      last_time = new_time;
    }
  }

  tex.reset();
  font.reset();
  Window::CloseWindow();
  return 0;
}
