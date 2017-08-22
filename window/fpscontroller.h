#ifndef EUPHORIA_FPSCONTROLLER_H
#define EUPHORIA_FPSCONTROLLER_H

#include "core/vec3.h"
#include "core/angle.h"
#include "core/quat.h"

class FpsController
{
 public:
  FpsController();

  void
  Look(float delta_rot, float delta_look);

  void
  MoveLeft(bool down);
  void
  MoveRight(bool down);
  void
  MoveForward(bool down);
  void
  MoveBackward(bool down);
  void
  MoveUp(bool down);
  void
  MoveDown(bool down);

  void
  HandleSdlKey(int key, bool down);

  void
  Update(float delta);

  vec3f
  GetPosition() const;
  void
  SetPosition(const vec3f& pos);
  quatf
  GetRotation() const;

 private:
  Angle rotation_;
  Angle look_;

  vec3f pos_;

  bool left_down_     = false;
  bool right_down_    = false;
  bool forward_down_  = false;
  bool backward_down_ = false;
  bool up_down_       = false;
  bool down_down_     = false;

  float speed_       = 3.0f;
  float sensitivity_ = 0.10f;
};


#endif  // EUPHORIA_FPSCONTROLLER_H
