#ifndef SPACETYPER_MAT4_H
#define SPACETYPER_MAT4_H

#include "spacetyper/vec3.h"
#include "spacetyper/vec4.h"
#include "spacetyper/angle.h"
#include "spacetyper/axisangle.h"

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
    const T z = 0;
    return FromRowMajor(
        one, z, z, v.x,
        z, one, z, v.y,
        z, z, one, v.z,
        z, z, z, one
    );
  }

  vec3<T> GetTransform(const vec3<T>& p) const {
    const vec4<T> r =  *this * vec4<T>(p, 1);
    return static_cast<vec3<T>>(r);
  }

  vec3<T> GetTranslation() const {
    return vec3<T>( Get(0,3), Get(1,3), Get(2,3) );
  }

  static mat4<T> FromScalar(T scalar) {
    const T z = 0;
    return FromRowMajor(
        scalar, z, z, z,
        z, scalar, z, z,
        z, z, scalar, z,
        z, z, z, scalar
    );
  }

  static mat4<T> FromRotX(const Angle& a) {
    const auto c = Cos(a);
    const auto s = Sin(a);
    return FromRowMajor(
        1,  0,  0,  0,
        0,  c, -s,  0,
        0,  s,  c,  0,
        0,  0,  0,  1
    );
  }

  static mat4<T> FromRotY(const Angle& a) {
    const auto c = Cos(a);
    const auto s = Sin(a);
    return FromRowMajor(
             c,  0,   s,  0,
             0,  1,   0,  0,
            -s,  0,   c,  0,
             0,  0,   0,  1
    );
  }

  static mat4<T> FromRotZ(const Angle& a) {
    const auto c = Cos(a);
    const auto s = Sin(a);
    return FromRowMajor(
           c,  -s,   0,   0,
           s,   c,   0,   0,
           0,   0,   1,   0,
           0,   0,   0,   1
    );
  }

  static mat4<T> FromAxisAngle(const AxisAngle aa) {
    const T rcos = Cos(aa.angle);
    const T rsin = Sin(aa.angle);
    mat4<T> matrix;
#define u aa.axis.x
#define v aa.axis.y
#define w aa.axis.z
    return mat4<T>::FromColMajor(
          rcos + u*u*(1-rcos),
      w * rsin + v*u*(1-rcos),
     -v * rsin + w*u*(1-rcos),
     0,

     -w * rsin + u*v*(1-rcos),
          rcos + v*v*(1-rcos),
      u * rsin + w*v*(1-rcos),
      0,

     v * rsin + u*w*(1-rcos),
    -u * rsin + v*w*(1-rcos),
         rcos + w*w*(1-rcos),
    0,

    0,
    0,
    0,
    1
    );
#undef u
#undef v
#undef w
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

  mat4<T> Rotate(const AxisAngle& aa) const {
    return *this * FromAxisAngle(aa);
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

  T Get(int row, int col) const {
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

template<typename T>
vec4<T> operator*(const mat4<T>& lhs, const vec4<T> rhs) {
#define OP(r) ComponentMultiply(lhs.GetRow(r), rhs).GetComponentSum()
  return vec4<T>(
      OP(0),
      OP(1),
      OP(2),
      OP(3)
  );
#undef OP
}

typedef mat4<float> mat4f;
typedef mat4<int> mat4i;

#endif  // SPACETYPER_MAT4_H