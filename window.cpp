#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace {
int window_width = 0;
int window_height = 0;
GLFWwindow* window = nullptr;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  printf("resize %d %d\n", width, height);
  window_width = width;
  window_height = height;
  glViewport(0, 0, width, height);
}
}  // namespace

bool InitWindow(int width, int height, const char *title) {
  if (!glfwInit()) {
    std::cout << "Failed to init GLFW" << std::endl;
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);
  window = glfwCreateWindow(window_width = width, window_height = height, title, nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  FramebufferSizeCallback(window, window_width, window_height);
  glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
  return true;
}

void CloseWindow() {
  glfwTerminate();
}

bool WindowShouldClose() {
  return glfwWindowShouldClose(window);
}

void SwapScreenBuffer() {
  glfwSwapBuffers(window);
}

void PollInputEvents() {
  glfwPollEvents();
}

double GetTime() {
  return glfwGetTime();
}

double GetWindowWidth() {
  return window_width;
}

double GetWindowHeight() {
  return window_height;
}
