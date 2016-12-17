#ifndef CORE_NONCOPYABLE_H
#define CORE_NONCOPYABLE_H

class Noncopyable {
 public:
  Noncopyable() {}
 private:
  Noncopyable(const Noncopyable& other);
  void operator=(const Noncopyable&);

  Noncopyable(Noncopyable&& other);
  void operator=(Noncopyable&&);
};

#endif  // CORE_NONCOPYABLE_H
