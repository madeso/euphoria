#ifndef RENDER_ID_H
#define RENDER_ID_H

#include "render/gltypes.h"

class Id {
 public:
  Id();
  ~Id();

  gluint id() const;

 protected:
  gluint id_;
};

#endif  // RENDER_ID_H
