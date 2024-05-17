#include <cassert>
#include <cmath>

const float EPSILON = 1e-6;

class vec2f {
public:
  float x = 0;
  float y = 0;

  vec2f(float x, float y) : x(x), y(y) {}

  static vec2f zero() { return vec2f(0.0, 0.0); }

  static vec2f one() { return vec2f(1.0, 1.0); }

  float dot(const vec2f &v) const { return x * v.x + y * v.y; }

  float length2() const { return dot(*this); }

  float length() const { return std::sqrt(length2()); }

  vec2f normalize() const {
    float l = length();
    assert(l > EPSILON);
    return vec2f(x / l, y / l);
  }

  vec2f operator+(const vec2f &rhs) const {
    return vec2f(x + rhs.x, y + rhs.y);
  }

  vec2f operator-(const vec2f &rhs) const {
    return vec2f(x - rhs.x, y - rhs.y);
  }

  vec2f operator*(float rhs) const { return vec2f(x * rhs, y * rhs); }

  vec2f operator/(float rhs) const { return vec2f(x / rhs, y / rhs); }
};
