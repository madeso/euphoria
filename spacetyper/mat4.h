#ifndef SPACETYPER_MAT4_H
#define SPACETYPER_MAT4_H

#include "spacetyper/vec3.h"
#include "spacetyper/vec4.h"

#include <vector>

template<typename T>
class mat4 {
 private:
  T data[16]; // col-major
  mat4() {}
  mat4(T t00, T t01, T t02, T t03,
       T t10, T t11, T t12, T t13,
       T t20, T t21, T t22, T t23,
       T t30, T t31, T t32, T t33) :
      data{t00, t01, t02, t03,
            t10, t11, t12, t13,
            t20, t21, t22, t23,
            t30, t31, t32, t33} {}
 public:
  static mat4<T> FromColMajor(T t00, T t01, T t02, T t03,
                              T t10, T t11, T t12, T t13,
                              T t20, T t21, T t22, T t23,
                              T t30, T t31, T t32, T t33) {
    return mat4<T>(t00, t01, t02, t03,
                   t10, t11, t12, t13,
                   t20, t21, t22, t23,
                   t30, t31, t32, t33);
  }
  static mat4<T> FromRowMajor(T t00, T t10, T t20, T t30,
                              T t01, T t11, T t21, T t31,
                              T t02, T t12, T t22, T t32,
                              T t03, T t13, T t23, T t33) {
    return mat4<T>(t00, t01, t02, t03,
                   t10, t11, t12, t13,
                   t20, t21, t22, t23,
                   t30, t31, t32, t33);
  }

  static mat4<T> FromMajor(const vec4<T>& major) {
    const T zero = 0;
    return FromRowMajor(
        major.x, zero, zero, zero,
        zero, major.y, zero, zero,
        zero, zero, major.z, zero,
        zero, zero, zero, major.w
    );
  }

  static mat4<T> FromScale(const vec3<T>& scale) {
    return FromMajor(vec4<T>(scale, 1));
  }

  static mat4<T> FromTranslation(const vec3<T>& v) {
    const T one = 1;
    const T zero = 0;
    return FromRowMajor(
        one, zero, zero, v.x,
        zero, one, zero, v.y,
        zero, zero, one, v.z,
        zero, zero, zero, one
    );
  }

  static mat4<T> FromScalar(T scalar) {
    const T zero = 0;
    return FromRowMajor(
        scalar, zero, zero, zero,
        zero, scalar, zero, zero,
        zero, zero, scalar, zero,
        zero, zero, zero, scalar
    );
  }

  static mat4<T> FromRotX(T A) {
    return FromRowMajor(
        1,  0,       0,       0,
        0,  cos(A), -sin(A),  0,
        0,  sin(A),  cos(A),  0,
        0,  0,       0,       1
    );
  }

  static mat4<T> FromRotY(T A) {
    return FromRowMajor(
            cos(A),  0,   sin(A),  0,
            0,       1,   0,       0,
            -sin(A),  0,  cos(A),  0,
            0,       0,   0,       1
    );
  }

  static mat4<T> FromRotZ(T A) {
    return FromRowMajor(
           cos(A),  -sin(A),   0,   0,
           sin(A),   cos(A),   0,   0,
           0,        0,        1,   0,
           0,        0,        0,   1
    );
  }

  static mat4<T> FromAxisAngle(const vec3<T>& axis, const T angle) {
    const T rcos = cos(angle);
    const T rsin = sin(angle);
    mat4<T> matrix;
#define u axis.x
#define v axis.y
#define w axis.z
    matrix(0,0) =      rcos + u*u*(1-rcos);
    matrix(1,0) =  w * rsin + v*u*(1-rcos);
    matrix(2,0) = -v * rsin + w*u*(1-rcos);
    matrix(3,0) = 0;

    matrix(0,1) = -w * rsin + u*v*(1-rcos);
    matrix(1,1) =      rcos + v*v*(1-rcos);
    matrix(2,1) =  u * rsin + w*v*(1-rcos);
    matrix(3,1) =  0;

    matrix(0,2) =  v * rsin + u*w*(1-rcos);
    matrix(1,2) = -u * rsin + v*w*(1-rcos);
    matrix(2,2) =      rcos + w*w*(1-rcos);
    matrix(3,2) = 0;

    matrix(3,0) = 0;
    matrix(3,1) = 0;
    matrix(3,2) = 0;
    matrix(3,3) = 1;
#undef u
#undef v
#undef w
    return matrix;
  }

  static mat4<T> Identity() {
    return FromScalar(1);
  }

  vec4<T> GetMajor() const {
    const mat4<T>& self = *this;
    return vec4<T>(self(0,0), self(1,1), self(2,2), self(3,3));
  }

  vec3<T> GetAxis(int col) const {
    return static_cast<vec3<T>>(GetColumn(col));
  }

  vec3<T> GetXAxis() const {
    return GetAxis(0);
  }

  vec3<T> GetYAxis() const {
    return GetAxis(1);
  }

  vec3<T> GetZAxis() const {
    return GetAxis(2);
  }

  mat4<T> GetTransposed() const {
    const mat4<T>& self = *this;
    return FromColMajor(
        self(0,0), self(0,1), self(0,2), self(0,3),
        self(1,0), self(1,1), self(1,2), self(1,3),
        self(2,0), self(2,1), self(2,2), self(2,3),
        self(3,0), self(3,1), self(3,2), self(3,3)
    );
  }

  void operator+=(const mat4<T> rhs) {
#define OP(i) data[i] += rhs.data[i]
    OP(0);
    OP(1);
    OP(2);
    OP(3);

    OP(4);
    OP(5);
    OP(6);
    OP(7);

    OP(8);
    OP(9);
    OP(10);
    OP(11);

    OP(12);
    OP(13);
    OP(14);
    OP(15);
#undef OP
  }

  void operator-=(const mat4<T> rhs) {
#define OP(i) data[i] -= rhs.data[i]
    OP(0);
    OP(1);
    OP(2);
    OP(3);

    OP(4);
    OP(5);
    OP(6);
    OP(7);

    OP(8);
    OP(9);
    OP(10);
    OP(11);

    OP(12);
    OP(13);
    OP(14);
    OP(15);
#undef OP
  }

  static mat4<T> Ortho(T l, T r, T b, T t, T n, T f) {
    // http://www.songho.ca/opengl/gl_projectionmatrix.html
    return FromRowMajor(
        2/(r-l), 0, 0, -(r+l)/(r-l),
        0, 2/(t-b), 0, -(t+b)/(t-b),
        0, 0, -2/(f-n), -(f+n)/(f-n),
        0,0,0,1
    );
  }

  mat4<T> Translate(const vec3<T>& t) const {
    return *this * FromTranslation(t);
  }

  mat4<T> Rotate(T angle, const vec3<T>& axis) const {
    return *this * FromAxisAngle(axis, angle);
  }

  mat4<T> Scale(const vec3<T>& scale) const {
    return *this * FromScale(scale);
  }

  const T* GetDataPtr() const { return data; }
  T* GetDataPtr() { return data; }

  // index operator use () as [] only expects one argument
  T& operator()(int row, int col) {
    return data[col*4+row];
  }
  T operator()(int row, int col) const {
    return data[col*4+row];
  }

  vec4<T> GetColumn(int c) const {
    return vec4<T>(&data[c*4]);
  }

  vec4<T> GetRow(int r) const {
    const mat4<T>& self = *this;
    return vec4<T>( self(r,0), self(r,1), self(r,2), self(r,3) );
  }
};

template <typename T>
bool operator==(const mat4<T>& lhs, const mat4<T>& rhs) {
  return
      lhs.GetColumn(0) == rhs.GetColumn(0)
      && lhs.GetColumn(1) == rhs.GetColumn(1)
      && lhs.GetColumn(2) == rhs.GetColumn(2)
      && lhs.GetColumn(3) == rhs.GetColumn(3)
      ;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const mat4<T>& m)
{
  return stream << "(" << m.GetRow(0) << ", " << m.GetRow(1) << ", " << m.GetRow(2) << ", " << m.GetRow(3) << ")";
}

template<typename T>
mat4<T> operator+(const mat4<T>& lhs, const mat4<T> rhs) {
  mat4<T> t = lhs;
  t += rhs;
  return t;
}

template<typename T>
mat4<T> operator-(const mat4<T>& lhs, const mat4<T> rhs) {
  mat4<T> t = lhs;
  t -= rhs;
  return t;
}

template<typename T>
mat4<T> operator*(const mat4<T>& lhs, const mat4<T> rhs) {
#define OP(r, c) ComponentMultiply(lhs.GetRow(r), rhs.GetColumn(c)).GetComponentSum()
  return mat4<T>::FromRowMajor(
      OP(0,0), OP(0,1), OP(0,2), OP(0,3),
      OP(1,0), OP(1,1), OP(1,2), OP(1,3),
      OP(2,0), OP(2,1), OP(2,2), OP(2,3),
      OP(3,0), OP(3,1), OP(3,2), OP(3,3)
  );
#undef OP
}

typedef mat4<float> mat4f;
typedef mat4<int> mat4i;

#endif  // SPACETYPER_MAT4_H