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
};

#endif
