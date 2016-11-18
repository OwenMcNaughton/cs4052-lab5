#include "Tree.hh"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

Tree::Tree(const mat4& mat, int depth)
    : Model("trunk", "trunk", mat) {
  mat_ = scale(mat_, vec3(1.0f, 1.5f, 1.0f));
  mat4 bush1_mat(mat);
  bush1_mat = translate(bush1_mat, vec3(0, 2.5, 0));
  Model* bush = new Model("", "", bush1_mat);
  AddChild(bush);

  RecurseBuild(bush1_mat, bush, depth);

  update_func_ = [this] (float dt) {
    sway_ += dt / 2000.0f;
    mat4 ident(1.0f);
    ident = translate(ident, vec3(0, -2.5, 0));
    ident = rotate(ident, sin(sway_) / 1200.0f, vec3(0, 0, 1));
    ident = translate(ident, vec3(0, 2.5, 0));
    mat_ = mat_ * ident;
  };
}

void Tree::RecurseBuild(const mat4& root_mat, Model* bush, int depth) {
  int branch_count = depth;
  for (int i = 0; i < branch_count; i++) {
    mat4 branch_mat(root_mat);
    mat4 ident(1.0f);
    ident = translate(ident, vec3(0.0f, -2.5f, 0.0f));
    ident = scale(ident, vec3(0.75, 0.75, 0.75));
    ident = rotate(ident, radians(rand() % 150 - 75.0f), vec3(1, 0, 0));
    ident = rotate(ident, radians(rand() % 150 - 75.0f), vec3(0, 1, 0));
    ident = rotate(ident, radians(rand() % 150 - 75.0f), vec3(0, 0, 1));
    ident = translate(ident, vec3(0.0f, 2.5f, 0.0f));
    branch_mat *= ident;
    Model* branch = new Model("trunk", "trunk", branch_mat);

    branch->update_func_ = [branch] (float dt) {
      branch->sway_ += dt / 2000.0f;
      mat4 ident(1.0f);
      ident = translate(ident, vec3(0, -2.5, 0));
      ident = rotate(ident, sin(branch->sway_) / 1200.0f, vec3(0, 0, 1));
      ident = translate(ident, vec3(0, 2.5, 0));
      branch->mat_ = branch->mat_ * ident;
    };

    mat4 next_bush_mat(1.0f);
    next_bush_mat = translate(next_bush_mat, vec3(0, 2.5, 0));

    if (depth == 3) {
      next_bush_mat = scale(next_bush_mat, vec3(4, 4, 4));
      Model* next_bush = new Model("leaves", "leaves256", next_bush_mat);

      next_bush->update_func_ = [next_bush] (float dt) {
        next_bush->sway_ += dt / 2000.0f;
        float fac = cos(next_bush->sway_ - 0.3f) / 800;
        mat4 ident(1.0f);
        ident = scale(ident, vec3(1 + fac, 1 + fac, 1 + fac));
        next_bush->mat_ = next_bush->mat_ * ident;
      };

      bush->AddChild(next_bush);
    } else {
      bush->AddChild(branch);
      Model* next_bush = new Model("", "", next_bush_mat);
      branch->AddChild(next_bush);
      RecurseBuild(branch_mat, next_bush, depth - 2);
    }
  }

}
