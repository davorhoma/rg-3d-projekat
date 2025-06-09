#pragma once
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

void limitFrameRate(const double frame_duration, double* lastTime);