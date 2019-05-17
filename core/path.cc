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
    if(is_closed_ || i + 1 < points.size())
    {
      points[LoopIndex(i+1)] += delta;
    }
    if(is_closed_ || i > 0)
    {
      points[LoopIndex(static_cast<int>(i)-1)] += delta;
    }
  }
  else
  {
    // point is control point, move the opposite point
    const int corresponding_control_index = IsAnchorPoint(i+1) ? i+2 : i-2;
    const int anchor_index = IsAnchorPoint(i+1) ? i+1 : i-1;
    if(is_closed_ || corresponding_control_index >= 0 && corresponding_control_index < points.size())
    {
      const auto cci = LoopIndex(corresponding_control_index);
      const auto ai = LoopIndex(anchor_index);
      const auto distance = vec2f::FromTo(points[cci], points[ai]).GetLength();
      const auto direction = vec2f::FromTo(points[i], points[ai]).GetNormalized();
      points[cci] = points[ai] + distance*direction;
    }
  }
}

BezierSeg2 BezierPath2::GetPointsInSegment(size_t i) const
{
  const auto b = i*3;
  return {
    points[b+0], // anchor
    points[b+1], //  ^ control
    points[LoopIndex(b+3)], // anchor
    points[b+2]  //  ^ control
  };
}

size_t BezierPath2::GetNumberOfSegments() const
{
  return points.size()/3;
}

void BezierPath2::SetClosed(bool is_closed)
{
  if(is_closed_ == is_closed )
  {
    return;
  }

  is_closed_ = is_closed;

  if(is_closed)
  {
    //              anchor                    control                                anchor (again)
    const auto p1 = points[points.size()-1] + vec2f::FromTo(points[points.size()-2], points[points.size()-1]);
    const auto p2 = points[0              ] + vec2f::FromTo(points[1              ], points[0              ]);
    points.push_back(p1);
    points.push_back(p2);
  }
  else
  {
    points.pop_back();
    points.pop_back();
  }
}

void BezierPath2::ToggleClosed()
{
  SetClosed(!is_closed_);
}


size_t BezierPath2::LoopIndex(int i) const
{
  const auto s = points.size();
  return (s+i)%s;
}

