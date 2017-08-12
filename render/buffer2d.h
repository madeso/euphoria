#ifndef RENDER_BUFFER_2D_H
#define RENDER_BUFFER_2D_H

#include "render/buffer.h"

class BufferBuilder2d;

class Buffer2d
{
 public:
  Buffer2d(const BufferBuilder2d& bb);

  void
  Draw() const;

 private:
  unsigned int index_count_;
  Vbo          vbo_;
  Vao          vao_;
  Ebo          ebo_;
};

#endif  // RENDER_BUFFER_2D_H
