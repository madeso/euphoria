#ifndef SPACETYPER_NONCOPYABLE_H
#define SPACETYPER_NONCOPYABLE_H

class Noncopyable {
 public:
  Noncopyable() {}
 private:
  Noncopyable(const Noncopyable& other);
  void operator=(const Noncopyable&);

  Noncopyable(Noncopyable&& other);
  void operator=(Noncopyable&&);
};

#endif  // SPACETYPER_NONCOPYABLE_H
