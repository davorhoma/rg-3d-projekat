#include "FrameLimitter.h"

void limitFrameRate(const double FRAME_DURATION, double* lastTime)
{
	double currentTime = glfwGetTime();
    double elapsedTime = currentTime - *lastTime;

    if (elapsedTime < FRAME_DURATION) {
        std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_DURATION - elapsedTime));
        currentTime = glfwGetTime();
    }

    *lastTime = currentTime;
}