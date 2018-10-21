#include "window/fpscontroller.h"
#include <SDL.h>

FpsController::FpsController()
    : rotation_(0.0_rad)
    , look_(0.0_rad)
    , position(vec3f::Origo())
{
}

void
FpsController::Look(float x, float y)
{
  rotation_ += Angle::FromDegrees(-x * sensitivity);
  look_ += Angle::FromDegrees(-y * sensitivity);
}

void
FpsController::MoveLeft(bool down)
{
  left_down_ = down;
}
void
FpsController::MoveRight(bool down)
{
  right_down_ = down;
}
void
FpsController::MoveForward(bool down)
{
  forward_down_ = down;
}
void
FpsController::MoveBackward(bool down)
{
  backward_down_ = down;
}
void
FpsController::MoveUp(bool down)
{
  up_down_ = down;
}
void
FpsController::MoveDown(bool down)
{
  down_down_ = down;
}

void
FpsController::HandleSdlKey(int key, bool down)
{
  switch(key)
  {
    case SDLK_w:
    case SDLK_UP:
      MoveForward(down);
      return;
    case SDLK_s:
    case SDLK_DOWN:
      MoveBackward(down);
      return;
    case SDLK_a:
    case SDLK_LEFT:
      MoveLeft(down);
      return;
    case SDLK_d:
    case SDLK_RIGHT:
      MoveRight(down);
      return;

    case SDLK_SPACE:
      MoveUp(down);
      return;
    case SDLK_LCTRL:
      MoveDown(down);
      return;
  }
}

void
FpsController::Update(float delta)
{
  int forward = 0;
  int right   = 0;
  int up      = 0;

  if(forward_down_)
  {
    forward += 1;
  }
  if(backward_down_)
  {
    forward -= 1;
  }

  if(right_down_)
  {
    right += 1;
  }
  if(left_down_)
  {
    right -= 1;
  }

  if(up_down_)
  {
    up += 1;
  }
  if(down_down_)
  {
    up -= 1;
  }

  if(forward == 0 && right == 0 && up == 0)
  {
    return;
  }

  const vec3f input =
      GetRotation().RightUpIn(vec3f(right, up, forward)).GetNormalized();
  const vec3f movement = input * speed * delta;

  position += movement;
}

quatf
FpsController::GetRotation() const
{
  const auto rotation = quatf::FromAxisAngle(
      AxisAngle::RightHandAround(vec3f::YAxis(), rotation_));
  const auto look =
      quatf::FromAxisAngle(AxisAngle::RightHandAround(vec3f::XAxis(), look_));
  return rotation * look;
}
