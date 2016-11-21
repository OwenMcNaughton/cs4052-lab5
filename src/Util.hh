#ifndef UTIL_HH
#define UTIL_HH

class Util {
 public:
  static Util& U() {
    static Util *instance = new Util();
    return *instance;
  }

  Util();

  int GetWidth();
  int GetHeight();

  static int mode_;
  static int rabbit_count_;
  static const int kNormalMode = 0, kBoidsMode = 1;
};

#endif
