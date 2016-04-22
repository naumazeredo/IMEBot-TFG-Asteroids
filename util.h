#ifndef IMEBOT_UTIL_H
#define IMEBOT_UTIL_H

#include <cmath>

inline float clamp(float v) {
  return fabs(v) > 1.0f ? v/fabs(v) : v;
}

inline float degtorad(float deg) {
  return deg * M_PI / 180.0f;
}

/*
inline float minangle(float deltaAngle) {
  return fabs(deltaAngle) > M_PI ? (-deltaAngle / fabs(deltaAngle))*fabs(2*M_PI - fabs(deltaAngle)) : deltaAngle;
}
*/
inline float minangle(float deltaAngle) { return deltaAngle; }

#endif
