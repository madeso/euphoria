#include "core/path.h"

#include <array>

#include "core/range.h"
#include "core/log.h"

namespace euphoria::core
{

LOG_SPECIFY_DEFAULT_LOGGER("painter")

BezierPath2::BezierPath2(const vec2f& center)
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

void BezierPath2::AddPoint(const vec2f& p)
{
  const auto p2 = points[points.size()-2]; // control point
  const auto p3 = points[points.size()-1]; // anchor point

  const auto p4 = p3 + vec2f::FromTo(p2, p3);
  const auto p6 = p;
  const auto p5 = p4 + vec2f::FromTo(p4, p6);

  points.push_back(p4);
  points.push_back(p5);
  points.push_back(p6);

  if(autoset_)
  {
    AutoSetAffectedControlPoints(points.size()-1);
  }
}

bool BezierPath2::IsAnchorPoint(size_t i)
{
  return i % 3 == 0;
}

bool BezierPath2::IsControlPoint(size_t i)
{
  return !IsAnchorPoint(i);
}


void BezierPath2::MovePoint(int i, const vec2f& delta)
{
  if(autoset_ && IsControlPoint(i))
  {
    // that point is on autoset
    return;
  }

  const auto r = MakeRange(points);
  points[i] += delta;

  if(autoset_)
  {
    AutoSetAffectedControlPoints(i);
    return;
  }

  if(IsAnchorPoint(i))
  {
    // anchor point, move control points too
    if(is_closed_ || IsWithin(r, i + 1)) { points[LoopIndex(i+1)] += delta; }
    if(is_closed_ || IsWithin(r, i - 1)) { points[LoopIndex(i-1)] += delta; }
  }
  else
  {
    // point is control point, move the opposite point
    const int corresponding_control_index = IsAnchorPoint(i+1) ? i+2 : i-2;
    const int anchor_index =                IsAnchorPoint(i+1) ? i+1 : i-1;
    if(is_closed_ || IsWithin(r, corresponding_control_index))
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

    if(autoset_)
    {
      AutoSetAnchorControlPoints(0);
      AutoSetAnchorControlPoints(points.size()-3);
    }
  }
  else
  {
    points.pop_back();
    points.pop_back();

    if(autoset_)
    {
      AutoSetStartAndEndControlPoints();
    }
  }
}

void BezierPath2::ToggleClosed()
{
  SetClosed(!is_closed_);
}


void BezierPath2::SetAutoSetControlPoints(bool is_autoset)
{
  if(is_autoset == autoset_)
  {
    return;
  }

  autoset_ = is_autoset;

  if(autoset_)
  {
    AutoSetAllControlPoints();
  }
}

void BezierPath2::ToggleAutoSetControlPoints()
{
  SetAutoSetControlPoints(!autoset_);
}


size_t BezierPath2::LoopIndex(int i) const
{
  const auto s = points.size();
  return (s+i)%s;
}

void BezierPath2::AutoSetAffectedControlPoints(int updated_anchor_index)
{
  const auto r = MakeRange(points);
  for(int i=updated_anchor_index-3; i<=updated_anchor_index+3; i+=3)
  {
    if(is_closed_ || IsWithin(r, i))
    {
      AutoSetAnchorControlPoints(LoopIndex(i));
    }
  }

  // might be affected...
  AutoSetStartAndEndControlPoints();
}

void BezierPath2::AutoSetAllControlPoints()
{
  for(int i=0; i<points.size(); i+=3)
  {
    AutoSetAnchorControlPoints(i);
  }
  AutoSetStartAndEndControlPoints();
}

void BezierPath2::AutoSetStartAndEndControlPoints()
{
  if(is_closed_) { return; }

  for(int i=0; i<2; i+=1)
  {
    const auto b = std::array<size_t, 2>{0, points.size()-3}[i];
    points[b+1] = (points[b+0] + points[b+2])*0.5f;
  }
}

void BezierPath2::AutoSetAnchorControlPoints(int anchor_index)
{
  const auto r = MakeRange(points);
  const auto anchor_pos = points[anchor_index];
  auto dir = vec2f::Zero();
  auto distances = std::array<float, 2>{0, 0};
  
  auto f = [&](int scale, int dist_index)
  {
    const auto index = anchor_index-3*scale;
    if(is_closed_ || IsWithin(r, index))
    {
      auto offset = (vec2f::FromTo(anchor_pos, points[LoopIndex(index)])).GetNormalizedVec();
      dir += offset.second.vec() * scale;
      distances[dist_index] = offset.first * scale;
    }
  };
  f(1, 0);
  f(-1, 1);
  dir.Normalize();

  for(int i=0; i<2; i+=1)
  {
    const auto control_index = anchor_index + std::array<int, 2>{-1, 1}[i];
    if(is_closed_ || IsWithin(r, control_index))
    {
      points[LoopIndex(control_index)] = anchor_pos + dir * distances[i] * 0.5f;
    }
  }
}

}
