#include "deps/glad.h"
#include "window.h"
#include "shader.h"
#include "image.h"
#include "deps/stb_truetype.h"
#include "utf8.h"
#include "draw.h"
#include "font.h"
#include "vec.h"

#include <algorithm>
#include <iostream>
#include <cmath>
#include <memory>

#include <GLFW/glfw3.h>

UniqueTexture GetTextureFromFile(const char* filename) {
  Image image(filename);
  if (image.data()) {
    printf("width = %d, height = %d, channels = %d\n", image.width(), image.height(), image.channels());
    return UniqueTexture(image.width(), image.height(), image.channels(), image.data());
  }
  return UniqueTexture{};
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

  InitVAOVBO();

  Shader shader = ShaderManager::GetShader("shader.vs.glsl", "shader.fs.glsl");
  Shader shader3d = ShaderManager::GetShader("shader3d.vs.glsl", "shader3d.fs.glsl");
  shader3d.Use();

  UniqueTexture tex = GetTextureFromFile("awesomeface.png");
  if (!tex->NotNull()) {
    printf("Load texture error\n");
    printf("error: %d\n", glGetError());
  }

  UniqueTexture container_tex = GetTextureFromFile("container.jpg");
  if (!container_tex->NotNull()) {
    printf("Load texture error\n");
    printf("error: %d\n", glGetError());
  }

  std::unique_ptr<Font> font = GetFont();
  printf("error: %d\n", glGetError());

  int frame_cnt = 0;
  int last_frame_cnt = 0;
  double fps_last_time = Window::GetTime();
  double last_cursor_x = 0;
  double last_cursor_y = 0;
  {
    Window::CursorPos cursor_pos = Window::GetCursorPos();
    last_cursor_x = cursor_pos.x;
    last_cursor_y = cursor_pos.y;
  }
  double camera_pitch = 0.0;
  double camera_yaw = 0.0;
  Vec3f camera_pos(-3.0, 0.0, 0.0);
  double fov = 45.0;
  Window::DisableCursor();
  double event_last_time = 0;

  Vertices3UvNormal vertices3;
  vertices3.AddBox();
  vertices3.AddToBuffer();

  while (!Window::WindowShouldClose()) {
    glEnable(GL_DEPTH_TEST);
    ViewPort::SetViewPort(0, 0, Window::width(), Window::height());
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    double new_time = Window::GetTime();
    float delta_time = new_time - event_last_time;
    event_last_time = new_time;
    constexpr float kCameraSpeed = 1.0;
    if (Window::GetKey(GLFW_KEY_W) == GLFW_PRESS) {
      camera_pos += delta_time * kCameraSpeed * Vec3f(std::cos(camera_yaw), std::sin(camera_yaw), .0f);
    }
    if (Window::GetKey(GLFW_KEY_S) == GLFW_PRESS) {
      camera_pos += delta_time * kCameraSpeed * Vec3f(-std::cos(camera_yaw), -std::sin(camera_yaw), .0f);
    }
    if (Window::GetKey(GLFW_KEY_A) == GLFW_PRESS) {
      camera_pos += delta_time * kCameraSpeed * Vec3f(-std::sin(camera_yaw), std::cos(camera_yaw), .0f);
    }
    if (Window::GetKey(GLFW_KEY_D) == GLFW_PRESS) {
      camera_pos += delta_time * kCameraSpeed * Vec3f(std::sin(camera_yaw), -std::cos(camera_yaw), .0f);
    }
    if (Window::GetKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
      camera_pos += delta_time * kCameraSpeed * Vec3f(0.0, 0.0, 1.0);
    }
    if (Window::GetKey(GLFW_KEY_Z) == GLFW_PRESS) {
      camera_pos += delta_time * kCameraSpeed * Vec3f(0.0, 0.0, -1.0);
    }

    Window::CursorPos cursor_pos = Window::GetCursorPos();
    constexpr double kSensitivity = 0.002;
    camera_yaw -= (cursor_pos.x - last_cursor_x) * kSensitivity;
    camera_pitch -= (cursor_pos.y - last_cursor_y) * kSensitivity;
    camera_pitch = std::clamp(camera_pitch, -M_PI * 0.49, M_PI * 0.49);
    last_cursor_x = cursor_pos.x;
    last_cursor_y = cursor_pos.y;

    Vec3f camera_dir(std::cos(camera_pitch) * std::cos(camera_yaw),
                     std::cos(camera_pitch) * std::sin(camera_yaw),
                     std::sin(camera_pitch));
    Mat4f view = LookAt(camera_pos, camera_pos + camera_dir, Vec3f(0.0f, 0.0f, 1.0f));
    Mat4f projection = Perspective(fov * (M_PI / 180.0), ViewPort::size().height ? 1.0f * ViewPort::size().width / ViewPort::size().height : 1.0f, 0.1f, 100.0f);

    shader3d.Use();
    shader3d.setMat4f("transform", (view * projection).GetValuePtr());
    shader3d.setVec3f("light_pos", camera_pos.x, camera_pos.y, camera_pos.z);

    container_tex->Use();
    vertices3.DrawCall();

    glDisable(GL_DEPTH_TEST);

    double offset = new_time * 50;

    {
      std::vector<int> result;
      bool ok = DecodeUTF8(u8"绝𪩘多生怪柏1234567890中文测试The quick brown fox jumps over a lazy dog ij (1 + 2) * 3\n"
                             "自三峡七百里中，两岸连山，略无阙处。重岩叠嶂，隐天蔽日，自非亭午夜分，不见曦月。\n"
                             "至于夏水襄陵，沿溯阻绝。或王命急宣，有时朝发白帝，暮到江陵，其间千二百里，虽乘奔御风，不以疾也。\n"
                             "满招损，谦受益。\n"
                             "天地本不全，经文残缺也应不全之理，非人力所能为也。", &result);
      if (!ok) {
        printf("Decode utf8 failed\n");
        return 0;
      }
      font->Draw(result, 500, 50);
    }

    shader.Use();
    tex->Use();
    Vertices2UvRgba vertices;
    vertices.AddRect(offset, 0, 300 + offset, 300, 0, 0, 1, 1);
    vertices.Draw();

    Window::SwapScreenBuffer();
    Window::PollInputEvents();

    frame_cnt++;
    if (new_time - fps_last_time > 1.0) {
      printf("frame_cnt = %d\n", frame_cnt);
      printf("fps = %lf\n", (frame_cnt - last_frame_cnt) / (new_time - fps_last_time));
      fps_last_time = new_time;
      last_frame_cnt = frame_cnt;
    }
  }

  tex.Reset();
  container_tex.Reset();
  font.reset();
  Window::CloseWindow();
  return 0;
}
