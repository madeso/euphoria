#include "gtest/gtest.h"
#include "gmock/gmock-generated-matchers.h"

#include "core/mat4.h"
#include "core/numeric.h"

#include "tests/testbase.h"

#define GTEST(X) TEST(mat4, X)
using namespace testing;

GTEST(colmajor) {
  const auto m = mat4i::FromColMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15);
  EXPECT_EQ(vec4i(0, 1, 2, 3), m.GetColumn(0));
  EXPECT_EQ(vec4i(4, 5, 6, 7), m.GetColumn(1));
  EXPECT_EQ(vec4i(8, 9, 10, 11), m.GetColumn(2));
  EXPECT_EQ(vec4i(12, 13, 14, 15), m.GetColumn(3));
}

GTEST(rowmajor) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15);
  EXPECT_EQ(vec4i(0, 4, 8, 12), m.GetColumn(0));
  EXPECT_EQ(vec4i(1, 5, 9, 13), m.GetColumn(1));
  EXPECT_EQ(vec4i(2, 6, 10, 14), m.GetColumn(2));
  EXPECT_EQ(vec4i(3, 7, 11, 15), m.GetColumn(3));
}

GTEST(rowmajor_row) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15);
  EXPECT_EQ(vec4i(0, 1, 2, 3), m.GetRow(0));
  EXPECT_EQ(vec4i(4, 5, 6, 7), m.GetRow(1));
  EXPECT_EQ(vec4i(8, 9, 10, 11), m.GetRow(2));
  EXPECT_EQ(vec4i(12, 13, 14, 15), m.GetRow(3));
}

GTEST(identity) {
  const auto m = mat4i::FromRowMajor(1, 0, 0, 0,
                                     0, 1, 0, 0,
                                     0, 0, 1, 0,
                                     0, 0, 0, 1);
  const auto i = mat4i::Identity();
  EXPECT_EQ(i.GetColumn(0), m.GetColumn(0));
  EXPECT_EQ(i.GetColumn(1), m.GetColumn(1));
  EXPECT_EQ(i.GetColumn(2), m.GetColumn(2));
  EXPECT_EQ(i.GetColumn(3), m.GetColumn(3));
}

GTEST(index) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15);
  EXPECT_EQ(3, m(0, 3));
  EXPECT_EQ(15, m(3, 3));
}

GTEST(major) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15).GetMajor();
  EXPECT_EQ(vec4i(0, 5, 10, 15), m);
}


GTEST(transposed) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15)
                                     .GetTransposed();
  EXPECT_EQ(mat4i::FromColMajor(0, 1, 2, 3,
                                4, 5, 6, 7,
                                8, 9, 10, 11,
                                12, 13, 14, 15), m);
}

GTEST(axis) {
  const auto m = mat4i::Identity();
  EXPECT_EQ(vec3i(1, 0, 0), m.GetXAxis());
  EXPECT_EQ(vec3i(0, 1, 0), m.GetYAxis());
  EXPECT_EQ(vec3i(0, 0, 1), m.GetZAxis());
}

GTEST(multiply) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15)
      *
      mat4i::FromRowMajor(16, 17, 18, 19,
                          20, 21, 22, 23,
                          24, 25, 26, 27,
                          28, 29, 30, 31);
  // thanks wolfram alpha for the result
  // http://www.wolframalpha.com/widgets/view.jsp?id=cc71c2e95a80c217564d530fd8297b0e
  // simplify({{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}}.{{16,17,18,19},{20,21,22,23},{24,25,26,27},{28,29,30,31}})
  EXPECT_EQ(mat4i::FromRowMajor(152, 158, 164, 170,
                                504, 526, 548, 570,
                                856, 894, 932, 970,
                                1208, 1262, 1316, 1370), m);
}

GTEST(add) {
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15)
                 +
                 mat4i::FromRowMajor(16, 17, 18, 19,
                                     20, 21, 22, 23,
                                     24, 25, 26, 27,
                                     28, 29, 30, 31);
  EXPECT_EQ(mat4i::FromRowMajor(16, 18, 20, 22,
                                24, 26, 28, 30,
                                32, 34, 36, 38,
                                40, 42, 44, 46), m);
}

GTEST(sub) {
  const auto m = mat4i::FromRowMajor(16, 17, 18, 19,
                                     20, 21, 22, 23,
                                     24, 25, 26, 27,
                                     28, 29, 30, 31)
                 -
                 mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15);
  EXPECT_EQ(mat4i::FromRowMajor(16, 16, 16, 16,
                                16, 16, 16, 16,
                                16, 16, 16, 16,
                                16, 16, 16, 16), m);
}



GTEST(mat3) {
  const auto m4 = mat4i::FromColMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15);
  const auto m3 = m4.GetMat3();
  EXPECT_EQ(vec3i(0, 1, 2), m3.GetColumn(0));
  EXPECT_EQ(vec3i(4, 5, 6), m3.GetColumn(1));
  EXPECT_EQ(vec3i(8, 9, 10), m3.GetColumn(2));
}


GTEST(inverse) {
  const auto Mat0 = mat4f::FromColMajor(
    0.6f, 0.2f, 0.3f, 0.4f,
    0.2f, 0.7f, 0.5f, 0.3f,
    0.3f, 0.5f, 0.7f, 0.2f,
    0.4f, 0.3f, 0.2f, 0.6f);

  auto Inv0 = Mat0; // glm::inverse(Mat0);
  ASSERT_TRUE(Inv0.Invert());
  ASSERT_TRUE(Inv0.Invert());

  // EXPECT_EQ(Mat0, Inv0);
}






////////////////////////////////////////////////////////////////////////////////

GTEST(TestTransormation)
{
  EXPECT_EQ(vec3i(1,2,3), mat4i::FromTranslation(vec3i(1,2,3)).GetTranslation());
}


GTEST(TestRight)
{
  EXPECT_EQ(vec3i::XAxis(), mat4i::Identity().GetXAxis());
}


GTEST(TestUp)
{
  EXPECT_EQ(vec3i::YAxis(), mat4i::Identity().GetYAxis());
}


GTEST(TestIn)
{
  EXPECT_EQ(vec3i::ZAxis(), mat4i::Identity().GetZAxis());
}

struct TestRotationFixture : ::testing::Test
{
  mat4f start;
  AxisAngle aa;
  vec3f toTransform;
  vec3f result;

  TestRotationFixture()
      : start(mat4f::Identity())
      , aa(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(-90)))
      , toTransform(0, 0, -5)
      , result(5, 0, 0)
  {
  }
};

TEST_F(TestRotationFixture, TestRotationAxisAngle)
{
  const auto r = start
      .Rotate(aa)
      .GetTransform(toTransform);
  EXPECT_PRED_FORMAT2(almost_equal, result, r);
}

/*
TEST_F(TestRotationFixture, TestRotationQuat)
{
  const auto r = start
      .Rotate(cquat(aa))
      .GetTransform(toTransform);
  EXPECT_FLOAT_EQ(result, r);
}
*/

GTEST(TestCombined_RT)
{
  const auto r = mat4f::Identity()
      .Rotate(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(-90)))
      .Translate(vec3f(0, 0, -5))
      .GetTransform(vec3f(0, 0, 0));
  EXPECT_PRED_FORMAT2(almost_equal, vec3f(5, 0, 0), r);
}

GTEST(TestCombined2_RT)
{
  const auto r = mat4f::Identity()
      .Rotate(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(90)))
      .Translate(vec3f(0, 0, -5))
      .GetTransform(vec3f(0, 0, 0));
  EXPECT_PRED_FORMAT2(almost_equal, vec3f(-5, 0, 0), r);
}

GTEST(TestCombined_TR)
{
  const auto r = mat4f::Identity()
      .Translate(vec3f(0, 0, 5))
      .Rotate(AxisAngle::RightHandAround(vec3f::Up(), Angle::FromDegrees(-90)))
      .GetTransform(vec3f(0, 0, 0));
  EXPECT_PRED_FORMAT2(almost_equal, vec3f(0, 0, 5), r);
}

GTEST(TestTranslation)
{
  const vec3i r = mat4i::Identity()
      .Translate(vec3i(1, 2, 3))
      .GetTransform(vec3i(7, 8, 9));
  EXPECT_EQ(vec3i(8, 10, 12), r);
}


GTEST(TestIentityTransform)
{
  const vec3i r = mat4i::Identity().GetTransform(vec3i(1, 2, 3));
  EXPECT_EQ(vec3i(1, 2, 3), r);
}

GTEST(TestIentityMultiply)
{
  EXPECT_EQ(mat4i::Identity(), mat4i::Identity() * mat4i::Identity());
}

GTEST(TestVec4Multiply)
{
  const auto m = mat4i::FromRowMajor(0, 1, 2, 3,
                                     4, 5, 6, 7,
                                     8 , 9 ,10 , 11,
                                     12, 13, 14, 15)
                 * vec4i(16, 17, 18, 19);
  // simplify({{0,1,2,3},{4,5,6,7},{8,9,10,11},{12,13,14,15}}.{16,17,18,19})
  EXPECT_EQ(vec4i(110, 390, 670, 950), m);
}

