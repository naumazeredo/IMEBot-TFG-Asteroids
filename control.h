#ifndef IMEBOT_CONTROL_H
#define IMEBOT_CONTROL_H

#include <cmath>
#include <string>
#include "bot_interface.h"

struct Control {
  float kp, kd;
  float lastError, lastTime;

  Control(float kp, float kd) : kp(kp), kd(kd), lastError(0), lastTime(0)
  {}

  float pd(float error, float curTime) {
    float deltaTime = curTime - lastTime;

    if (fabs(deltaTime) < 0.001) return 0.0f;

    float derivate = (error - lastError) / deltaTime;

    lastTime = curTime;
    lastError = error;
    return kp * error + kd * derivate;
  }
};

#endif
