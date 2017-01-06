#ifndef CORE_SIZE_H
#define CORE_SIZE_H

template<typename T>
class Size {
public:
  typedef Size<T> Self;
  Size() : width_(-1), height_(-1) {}
  static Self FromWidthHeight(T w, T h) { return Self(w, h); }
  static Self FromHeightWidth(T h, T w) { return Self(w, h); }
  static Self FromSquare(T s) { return Self(s, s); }

  T GetWidth() const {
    return width_;
  }

  void SetWidth(T w) {
    width_ = w;
  }

  T GetHeight() const {
    return height_;
  }

  void SetHeight(T h) {
    height_ = h;
  }

private:
  Size(T w, T h) : width_(w), height_(h) {}
  T width_;
  T height_;
};

typedef Size<int> Sizei;
typedef Size<float> Sizef;

#endif // CORE_SIZE_H
