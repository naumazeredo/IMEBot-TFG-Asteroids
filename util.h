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

struct vec2 { float x, y; };

inline vec2 operator-(const vec2 a) { return {-a.x, -a.y}; }
inline vec2 operator+(const vec2 a, const vec2 b) { return {a.x + b.x, a.y + b.y}; }
inline vec2 operator-(const vec2 a, const vec2 b) { return a+(-b); }
inline vec2 operator*(const vec2 v, const float s) { return {v.x * s, v.y * s}; }
inline vec2 operator/(const vec2 v, const float s) { return {v.x / s, v.y / s}; }
inline vec2& operator+=(vec2& a, const vec2 b) { return a=a+b; }
inline vec2& operator-=(vec2& a, const vec2 b) { return a=a-b; }
inline vec2& operator*=(vec2& a, const float s) { return a=a*s; }
inline vec2& operator/=(vec2& a, const float s) { return a=a/s; }
inline float mag(const vec2& a) { return sqrt((a.x * a.x) + (a.y * a.y)); }

#endif
