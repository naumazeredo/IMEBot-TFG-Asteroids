#ifndef IMEBOT_UTIL_H
#define IMEBOT_UTIL_H

#include <cmath>

inline float clamp(float v) {
  return fabs(v) > 1.0f ? v/fabs(v) : v;
}

inline float degtorad(float deg) {
  return deg * M_PI / 180.0f;
}

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
inline float squaremag(const vec2& a) { return (a.x * a.x) + (a.y * a.y); }
inline vec2 norm(const vec2& a) { return a/mag(a); }
inline float dot(const vec2& a, const vec2& b) { return (a.x * b.x) + (a.y * b.y); }
inline vec2 rotate(const vec2& a, const float angle) { return {a.x * (float)cos(angle) - a.y * (float)sin(angle), a.x * (float)sin(angle) + a.y * (float)cos(angle)}; }
inline vec2 project(const vec2& a, const float angle) { return {mag(a) * (float)sin(angle), mag(a) * (float)cos(angle)}; }
inline float cross(const vec2& a, const vec2& b) { return (a.x * b.y) - (a.y * b.x); }

#endif
