#include "core/path.h"

BezierPath2::BezierPath2(const point2f& center)
{
  const auto left = vec2f(-1, 0);
  const auto right = vec2f(1, 0);
  const auto up = vec2f(0, 1);
  const auto down = vec2f(0, -1);

  points.push_back(center + left);
  points.push_back(center + (left+up)*0.5f);
  points.push_back(center + (right+down)*0.5f);
  points.push_back(center + right);
}

void BezierPath2::AddPoint(const point2f& p)
{
  const auto p2 = points[points.size()-2]; // control point
  const auto p3 = points[points.size()-1]; // anchor point

  const auto p4 = p3 + vec2f::FromTo(p2, p3);
  const auto p6 = p;
  const auto p5 = p4 + vec2f::FromTo(p4, p6);

  points.push_back(p4);
  points.push_back(p5);
  points.push_back(p6);
}

bool BezierPath2::IsAnchorPoint(size_t i)
{
  return i % 3 == 0;
}

bool BezierPath2::IsControlPoint(size_t i)
{
  return !IsAnchorPoint(i);
}


void BezierPath2::MovePoint(size_t i, const vec2f& delta)
{
  points[i] += delta;

  if(IsAnchorPoint(i))
  {
    // anchor point, move control points too
    if(i + 1 < points.size())
    {
      points[i+1] += delta;
    }
    if(i > 0)
    {
      points[i-1] += delta;
    }
  }
  else
  {
    // point is control point, move the opposite point
  }
}

BezierSeg2 BezierPath2::GetPointsInSegment(size_t i) const
{
  const auto b = i*3;
  return {
    points[b+0], // anchor
    points[b+1], //  ^ control
    points[b+3], // anchor
    points[b+2]  //  ^ control
  };
}

size_t BezierPath2::GetNumberOfSegments() const
{
  return points.size()/3;
}

