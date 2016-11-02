

/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <iostream>

#include "spacetyper/texture.h"
#include "spacetyper/image.h"

Texture2dLoadData::Texture2dLoadData() : wrapS(GL_REPEAT), wrapT(GL_REPEAT), filterMin(GL_LINEAR), filterMax(GL_LINEAR) {
}

////////////////////////////////////////////////////////////////////////////////

TextureId::TextureId() {
  std::cout << "Allocating texture\n";
  glGenTextures(1, &id_);
}

TextureId::~TextureId() {
  std::cout << "Removing texture.\n";
  glDeleteTextures(1, &id_);
}

void TextureId::Bind() const
{
  glBindTexture(GL_TEXTURE_2D, id_);
}

////////////////////////////////////////////////////////////////////////////////

Texture2d::Texture2d()
    : width_(0), height_(0) {
}

Texture2d::Texture2d(const std::string& path)
    : width_(0), height_(0) {
  LoadFromFile(path, AlphaLoad::Include, Texture2dLoadData());
}

Texture2d::Texture2d(const std::string& path, AlphaLoad alpha, const Texture2dLoadData& data)
    : width_(0), height_(0) {
  LoadFromFile(path, alpha, data);
}

Texture2d::~Texture2d() {
}

void Texture2d::Load(int width, int height, unsigned char* pixelData, GLuint internalFormat, GLuint imageFormat, const Texture2dLoadData& data) {
  Bind();
  glTexImage2D(GL_TEXTURE_2D, 0, data.internalFormat, width, height, 0, data.imageFormat, GL_UNSIGNED_BYTE, pixelData);

  width_ = width;
  height_ = height;

  std::cout << "Loading texture\n";

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, data.wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, data.wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, data.filterMin);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, data.filterMax);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2d::LoadFromFile(const std::string& path, AlphaLoad alpha, const Texture2dLoadData& data) {
  ImageLoadResult i = LoadImage(path, alpha);
  if(i.height <= 0 ) {
    std::cerr << "Failed to load image " << path << "\n"
              << "  " << i.error << "\n";
    return;
  }

  std::cout << "Loading image from "<< path << "\n";

  GLuint internalFormat = GL_RGB;
  GLuint imageFormat = GL_RGB;
  if (i.has_alpha)
  {
    internalFormat = GL_RGBA;
    imageFormat = GL_RGBA;
  }

  Load(i.width, i.height, &i.components[0], internalFormat, imageFormat, data);
}
