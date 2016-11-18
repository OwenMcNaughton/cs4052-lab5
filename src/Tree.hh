#ifndef TREE_HH
#define TREE_HH

#include <glm/glm.hpp>

#include "Model.hh"

using namespace glm;

class Tree : public Model {
 public:
  Tree(const mat4& mat, int depth);

  void RecurseBuild(const mat4& root_mat, Model* bush, int depth);

  void Sway(int dt);
};

#endif
