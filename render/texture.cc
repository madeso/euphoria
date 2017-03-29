
#include <iostream>

#include "core/image.h"
#include "render/texture.h"
#include "render/gl.h"

Texture2dLoadData::Texture2dLoadData()
    : wrapS(GL_REPEAT),
      wrapT(GL_REPEAT),
      filterMin(GL_LINEAR),
      filterMax(GL_LINEAR) {}

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

Texture2d::Texture2d(const std::string& path) : width_(0), height_(0) {
  LoadFromFile(path, AlphaLoad::Include, Texture2dLoadData());
}

Texture2d::Texture2d(const std::string& path, AlphaLoad alpha,
                     const Texture2dLoadData& data)
    : width_(0), height_(0) {
  LoadFromFile(path, alpha, data);
}

void Texture2d::Load(int width, int height, unsigned char* pixelData,
                     GLuint internalFormat, GLuint imageFormat,
                     const Texture2dLoadData& data) {
  Use(this);
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat,
               GL_UNSIGNED_BYTE, pixelData);

  width_ = width;
  height_ = height;

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, data.wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, data.wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data.filterMin);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, data.filterMax);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2d::LoadFromFile(const std::string& path, AlphaLoad alpha,
                             const Texture2dLoadData& data) {
  ImageLoadResult i = LoadImage(path, alpha);
  if (i.height <= 0) {
    std::cerr << "Failed to load image " << path << "\n"
              << "  " << i.error << "\n";
    return;
  }

  GLuint internalFormat = GL_RGB;
  GLuint imageFormat = GL_RGB;
  if (i.has_alpha) {
    internalFormat = GL_RGBA;
    imageFormat = GL_RGBA;
  }

  Load(i.width, i.height, &i.components[0], internalFormat, imageFormat, data);
}

int Texture2d::width() const { return width_; }

int Texture2d::height() const { return height_; }
