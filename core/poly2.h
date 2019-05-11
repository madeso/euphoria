
#ifndef CORE_POLY2_H
#define CORE_POLY2_H

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

struct VertexPath2
{
  std::vector<ve2f> points;
  bool is_closed = false;
};


#endif  // CORE_POLY_H
