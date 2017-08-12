#ifndef RENDER_ID_H
#define RENDER_ID_H

#include "core/noncopyable.h"
#include "render/gltypes.h"

class Id : Noncopyable
{
 public:
  Id();
  ~Id();

  gluint
  id() const;

 protected:
  gluint id_;
};

#endif  // RENDER_ID_H
