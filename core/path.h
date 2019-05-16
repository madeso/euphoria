#ifndef CORE_PATH_H
#define CORE_PATH_H

#include <vector>
#include "core/vec2.h"

// todo: a poly is a closed path, so rename to path
// todo: add more features
// sebastian lague: bezier support https://github.com/SebLague/Path-Creator
// paper.js, smooth and simpify(path fitter) path functions: https://github.com/paperjs/paper.js/blob/develop/src/path/PathFitter.js https://github.com/paperjs/paper.js/blob/develop/src/path/PathItem.js#L466
// path fitter original code: https://github.com/erich666/GraphicsGems/blob/master/gems/FitCurves.c

// suggested new names:
// bezier path - a bezier path that can either be open or closed, has anchor points, control points
// vertex path - a path with points with straight linear interpolated lines, can either be open or closed
// both paths can either be concave or convex
//
// can use a tesselator function to create triangulated/convex polygons from closed paths with closed paths as holes:
// https://github.com/memononen/libtess2
// https://github.com/SebLague/Shape-Editor-Tool/blob/master/Shape%20Editor%20E04/Assets/Geometry/Triangulator.cs
//   -> https://www.geometrictools.com/Documentation/TriangulationByEarClipping.pdf

// check if point in poly: http://geomalgorithms.com/a03-_inclusion.html

struct BezierSeg2
{
  point2f a0; point2f c0;
  point2f a1; point2f c1;
};

struct BezierPath2
{
  std::vector<point2f> points;

  BezierPath2(const point2f& center);

  static bool IsAnchorPoint(size_t i);
  static bool IsControlPoint(size_t i);

  // point functions
  void AddPoint(const point2f& p);
  void MovePoint(size_t i, const vec2f& delta);

  // segment functions
  size_t GetNumberOfSegments() const;
  BezierSeg2 GetPointsInSegment(size_t i) const;
};

struct VertexPath2
{
  std::vector<point2f> points;
  bool is_closed = false;
};


#endif  // CORE_PATH_H

