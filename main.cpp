#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "image.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "deps/stb_truetype.h"

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

constexpr float mx = 0.1;

Vertex vertices[] = {
  Vertex{.x = -0.5, .y = -0.5, .u = 0.0, .v = mx},
  Vertex{.x =  0.5, .y = -0.5, .u = mx, .v = mx},
  Vertex{.x =  0.5, .y =  0.5, .u = mx, .v = 0.0},
  Vertex{.x =  0.5, .y =  0.5, .u = mx, .v = 0.0},
  Vertex{.x = -0.5, .y =  0.5, .u = 0.0, .v = 0.0},
  Vertex{.x = -0.5, .y = -0.5, .u = 0.0, .v = mx},
};

void GetFont() {
  constexpr int texsz = 8192;
  std::vector<unsigned char> ttf_buffer(20 << 20);
  std::vector<unsigned char> temp_bitmap(texsz * texsz);

  FILE* file = fopen("SourceHanSansSC-Normal.otf", "rb");
  int sz = fread(ttf_buffer.data(), 1, 20 << 20, file);
  printf("sz = %d\n", sz);
  fclose(file);

  constexpr int cjkstart = 0x4e00;
  constexpr int cjkend = 0x9FAF;

  stbtt_packedchar packed_chars1[96];
  stbtt_packedchar packed_chars2[cjkend - cjkstart];

  stbtt_pack_context spc = {};
  stbtt_PackBegin(&spc, temp_bitmap.data(), texsz, texsz, 0 /* stride */, 1 /* padding */, NULL);
  stbtt_PackFontRange(&spc, ttf_buffer.data(), 0, 36.0, 32, 96, packed_chars1);
  stbtt_PackFontRange(&spc, ttf_buffer.data(), 0, 36.0, cjkstart, cjkend - cjkstart, packed_chars2);
  stbtt_PackEnd(&spc);

  GLuint ftex;
  glGenTextures(1, &ftex);
  glBindTexture(GL_TEXTURE_2D, ftex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texsz, texsz, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap.data());
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

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, x));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, u));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)offsetof(Vertex, r));
  glEnableVertexAttribArray(2);

  GetFont();

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
    glDrawArrays(GL_TRIANGLES, 0, 6);
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
