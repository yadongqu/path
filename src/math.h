#pragma once
#include <cassert>
#include <cmath>
#include <iostream>
#include <ostream>

namespace Flow {

const float EPSILON = 1e-6;

struct Vec2f {
  float x = 0;
  float y = 0;

  Vec2f(float x, float y) : x(x), y(y) {}

  static Vec2f zero() { return Vec2f(0.0, 0.0); }

  static Vec2f one() { return Vec2f(1.0, 1.0); }

  float dot(const Vec2f &v) const { return x * v.x + y * v.y; }

  float length2() const { return dot(*this); }

  float length() const { return std::sqrt(length2()); }

  Vec2f normalize() const {
    float l = length();
    assert(l > EPSILON);
    return Vec2f(x / l, y / l);
  }

  Vec2f operator+(const Vec2f &rhs) const {
    return Vec2f(x + rhs.x, y + rhs.y);
  }

  Vec2f operator-(const Vec2f &rhs) const {
    return Vec2f(x - rhs.x, y - rhs.y);
  }

  Vec2f operator*(float rhs) const { return Vec2f(x * rhs, y * rhs); }

  Vec2f operator/(float rhs) const { return Vec2f(x / rhs, y / rhs); }
};

std::ostream &operator<<(std::ostream &os, const Vec2f &v) {
  os << "Vec2f(" << v.x << ", " << v.y << ")";
  return os;
}

struct Vec3f {
  float x = 0.0;
  float y = 0.0;
  float z = 0.0;

  Vec3f operator-() const { return Vec3f(-x, -y, -z); }

  Vec3f operator+(const Vec3f &rhs) const {
    return Vec3f(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  Vec3f operator-(const Vec3f &rhs) const {
    return Vec3f(x - rhs.x, y - rhs.y, z - rhs.z);
  }

  Vec3f operator*(float scalar) const {
    return Vec3f(x * scalar, y * scalar, z * scalar);
  }

  Vec3f operator/(float scalar) const {
    return Vec3f(x / scalar, y / scalar, z / scalar);
  }

  Vec3f &operator+=(const Vec3f &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  float dot(const Vec3f &v) const { return x * v.x + y * v.y + z * v.z; }

  Vec3f reflect(const Vec3f &d, const Vec3f &n) const {
    return d - (n * (d.dot(n) * 2.0));
  }

  static Vec3f zero() { return Vec3f(0.0, 0.0, 0.0); }

  static Vec3f one() { return Vec3f(1.0, 1.0, 1.0); }

  float length2() const { return dot(*this); }

  float length() const { return std::sqrt(length2()); }

  Vec3f normalize() const {
    float l = length();
    if (l < EPSILON) {
      std::cout << l << std::endl;
    }
    assert(l > EPSILON);
    return *this * (1.0 / l);
  }

  float mean() const { return (x + y + z) / 3.0; }

  Vec3f abs() const { return Vec3f(std::abs(x), std::abs(y), std::abs(z)); }

  bool is_near_zero() const {
    return std::abs(x) < EPSILON && std::abs(y) < EPSILON &&
           std::abs(z) < EPSILON;
  }

  Vec3f cross(const Vec3f &v) const {
    return Vec3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }
};

std::ostream &operator<<(std::ostream &os, const Vec3f &v) {
  os << "Vec3f(" << v.x << ", " << v.y << ", " << v.z << ")";
  return os;
}

class Vec4f {
public:
  float x;
  float y;
  float z;
  float w;

  Vec4f(float x = 0.0, float y = 0.0, float z = 0.0, float w = 0.0)
      : x(x), y(y), z(z), w(w) {}

  static Vec4f zero() { return Vec4f(0.0, 0.0, 0.0, 0.0); }

  static Vec4f one() { return Vec4f(1.0, 1.0, 1.0, 1.0); }

  float dot(const Vec4f &v) const {
    return x * v.x + y * v.y + z * v.z + w * v.w;
  }

  Vec3f to_vec3() const { return Vec3f{.x = x, .y = y, .z = z}; }
};

std::ostream &operator<<(std::ostream &os, const Vec4f &v) {
  os << "Vec4f(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
  return os;
}
} // namespace Flow