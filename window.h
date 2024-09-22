#pragma once

class Window {
  static int width_, height_;
  class Internal;
public:
  static int width() { return width_; }
  static int height() { return height_; }
  static bool InitWindow(int width, int height, const char *title);
  static void CloseWindow();
  static bool WindowShouldClose();
  static void SwapScreenBuffer();
  static void PollInputEvents();
  static double GetTime();
  static void DisableCursor();
  static int GetKey(int key);

  struct CursorPos { double x = 0, y = 0; };
  static CursorPos GetCursorPos();
};
