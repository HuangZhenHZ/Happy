#pragma once

struct GLFWwindow;

class Window {
  static int width_, height_;
  static GLFWwindow *window_;
public:
  static int width() { return width_; }
  static int height() { return height_; }
  static bool InitWindow(int width, int height, const char *title);
  static void CloseWindow();
  static bool WindowShouldClose();
  static void SwapScreenBuffer();
  static void PollInputEvents();
  static double GetTime();
private:
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};
