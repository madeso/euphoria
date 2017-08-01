#ifndef TEXTURE_H
#define TEXTURE_H

#include "core/image.h"
#include "render/gltypes.h"
#include "core/noncopyable.h"

struct Texture2dLoadData {
  Texture2dLoadData();

  gluint wrapS;
  gluint wrapT;
  gluint filterMin;
  gluint filterMax;
};

class TextureId : Noncopyable {
 public:
  TextureId();
  ~TextureId();

  bool IsCurrentlyBound() const;
  gluint id() const;

 private:
  gluint id_;
};

void Use(const TextureId* texture);

class Texture2d : public TextureId {
 public:
  Texture2d();

  // todo: there are too many contructors here, remove

  explicit Texture2d(const std::string& path);
  Texture2d(const std::string& path, AlphaLoad alpha,
            const Texture2dLoadData& data);

  void Load(int width, int height, const unsigned char* pixelData,
            gluint internalFormat, gluint imageFormat,
            const Texture2dLoadData& data);
  void LoadFromFile(const std::string& path, AlphaLoad alpha,
                    const Texture2dLoadData& data);

  int width() const;
  int height() const;

 private:
  int width_, height_;
};

#endif
