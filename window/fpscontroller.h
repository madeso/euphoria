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

  quatf
  GetRotation() const;

 private:
  Angle rotation_;
  Angle look_;

  bool left_down_     = false;
  bool right_down_    = false;
  bool forward_down_  = false;
  bool backward_down_ = false;
  bool up_down_       = false;
  bool down_down_     = false;

 public:
  point3f position;
  float speed       = 3.0f;
  float sensitivity = 0.10f;
};


#endif  // EUPHORIA_FPSCONTROLLER_H
