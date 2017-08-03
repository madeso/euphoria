
#include <iostream>

#include "core/image.h"
#include "render/texture.h"
#include "render/gl.h"
#include "core/assert.h"

namespace // local
{

gluint C(TextureWrap v) {
  switch(v) {
    case TextureWrap::REPEAT:
      return GL_REPEAT;
    case TextureWrap::MIRRORED_REPEAT:
      return GL_MIRRORED_REPEAT;
    case TextureWrap::CLAMP_TO_EDGE:
      return GL_CLAMP_TO_EDGE;
  }

  Assert(false);
  return GL_REPEAT;
}

gluint C(FilterMagnification v) {
  switch(v) {
    case FilterMagnification::NEAREST:
      return GL_NEAREST;
    case FilterMagnification::LINEAR:
      return GL_LINEAR;
  }


  Assert(false);
  return GL_LINEAR;
}

gluint C(FilterMinification v) {
  switch(v) {
    case FilterMinification::NEAREST:
      return GL_NEAREST;
    case FilterMinification::LINEAR:
      return GL_LINEAR;
  }

  Assert(false);
  return GL_LINEAR;
}
}


Texture2dLoadData::Texture2dLoadData()
    : wrap(TextureWrap::REPEAT),
      min(FilterMinification::LINEAR),
      mag(FilterMagnification ::LINEAR) {}

Texture2dLoadData& Texture2dLoadData::SetWrap(TextureWrap v) {
  wrap = v;
  return *this;
}

Texture2dLoadData& Texture2dLoadData::SetFilterMag(FilterMagnification v) {
  mag = v;
  return *this;
}

Texture2dLoadData& Texture2dLoadData::SetFilterMin(FilterMinification v) {
  min = v;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////

TextureId::TextureId() { glGenTextures(1, &id_); }

TextureId::~TextureId() { glDeleteTextures(1, &id_); }

GLuint TextureId::id() const { return id_; }

namespace {
const TextureId*& currentShader() {
  static const TextureId* s = nullptr;
  return s;
}
}

bool TextureId::IsCurrentlyBound() const { return this == currentShader(); }

void Use(const TextureId* texture) {
  if (texture != nullptr) {
    glBindTexture(GL_TEXTURE_2D, texture->id());
  }
  currentShader() = texture;
}

////////////////////////////////////////////////////////////////////////////////

Texture2d::Texture2d() : width_(0), height_(0) {}

void Texture2d::LoadFromPixels(int width, int height, const unsigned char* pixelData,
                     GLuint internalFormat, GLuint imageFormat,
                     const Texture2dLoadData& data) {
  Use(this);
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat,
               GL_UNSIGNED_BYTE, pixelData);

  width_ = width;
  height_ = height;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, C(data.wrap));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, C(data.wrap));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, C(data.min));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, C(data.mag));

  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2d::LoadFromFile(const std::string& path, AlphaLoad alpha,
                             const Texture2dLoadData& data) {
  ImageLoadResult i = LoadImage(path, alpha);
  if (!i.image.IsValid()) {
    std::cerr << "Failed to load image " << path << "\n"
              << "  " << i.error << "\n";
    return;
  }
  LoadFromImage(i.image, alpha, data);
}

void Texture2d::LoadFromImage(const Image& image, AlphaLoad alpha,
                   const Texture2dLoadData& data) {
  GLuint internalFormat = GL_RGB;
  GLuint imageFormat = GL_RGB;
  if (image.HasAlpha() && alpha == AlphaLoad::Include ) {
    internalFormat = GL_RGBA;
    imageFormat = GL_RGBA;
  }

  LoadFromPixels(image.GetWidth(), image.GetHeight(), image.GetPixelData(), internalFormat, imageFormat, data);
}

int Texture2d::width() const { return width_; }

int Texture2d::height() const { return height_; }
