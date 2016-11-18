#ifndef MODEL_HH
#define MODEL_HH

#include <functional>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

using namespace std;
using namespace glm;

class Model {
 public:
  Model(const string& mesh, const string& texture, const mat4& mat);

  virtual void Update(int dt);

  virtual void Render(const mat4& root, GLuint shader_program);

  void AddChild(Model* child);

  mat4 mat_;
  vector<Model*> children_;
  string mesh_;
  GLuint vert_buffer_, uv_buffer_, norm_buffer_, texture_;
  std::function<void(int)> update_func_;
  float sway_ = rand() % 1000;
  float alpha_ = 1.0;

 private:
  int vert_count_;
};

class AnimatedModel : public Model {
 public:
  AnimatedModel(
    const vector<string>& meshes,
    const string& texture,
    const mat4& mat,
    const vector<int>& tick_counts);

  virtual void Update(int dt) override;

  virtual void Render(const mat4& root, GLuint shader_program) override;

  int model_idx_ = 0, elapsed_ticks_ = 0;
  vector<int> tick_counts_;
  vector<Model> models_;
};

#endif
