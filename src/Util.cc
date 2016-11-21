#include "Util.hh"

int Util::mode_ = Util::kNormalMode;
int Util::rabbit_count_ = 300;

Util::Util() {

}

int Util::GetWidth() {
  return 1200;
}

int Util::GetHeight() {
  return 700;
}
