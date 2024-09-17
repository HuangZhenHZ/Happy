#include "window.h"
#include <GLFW/glfw3.h>
#include <cstdio>

int Window::width_ = 0;
int Window::height_ = 0;

namespace {
GLFWwindow *window;
}  // namespace

class Window::Internal {
  friend class Window;
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    printf("resize %d %d\n", width, height);
    Window::width_ = width;
    Window::height_ = height;
  }
};

bool Window::InitWindow(int width, int height, const char *title) {
  if (!glfwInit()) {
    puts("Failed to init GLFW");
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  window = glfwCreateWindow(width_ = width, height_ = height, title, nullptr, nullptr);
  if (window == nullptr) {
    puts("Failed to create GLFW window");
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(0);
  Internal::FramebufferSizeCallback(window, width_, height_);
  glfwSetFramebufferSizeCallback(window, Internal::FramebufferSizeCallback);
  return true;
}

void Window::CloseWindow() {
  glfwTerminate();
}

bool Window::WindowShouldClose() {
  return glfwWindowShouldClose(window);
}

void Window::SwapScreenBuffer() {
  glfwSwapBuffers(window);
}

void Window::PollInputEvents() {
  glfwPollEvents();
}

double Window::GetTime() {
  return glfwGetTime();
}
