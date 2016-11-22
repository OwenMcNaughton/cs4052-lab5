#ifndef BOID_HH
#define BOID_HH

#include <map>
#include "Model.hh"
#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;
using namespace std;

const int kRabbit = 0;
const int kFox = 1;

class Boid {
 public:
  Boid(const mat4& mat, int type);

  bool Update(int dt);

  void Render(const mat4& root, GLuint shader_program);

  void Flock(
    int dt, const vector<Boid>& prey, const vector<Boid>& predators,
    const vector<vec3>& obstacles, const vec3& center);

  vec3 Separation(const vector<Boid>& others, float min_dist);
  vec3 Cohesion(const vector<Boid>& others, float min_dist);
  vec3 Alignment(const vector<Boid>& others);
  vec3 Obstacles(const vector<vec3>& obstacles);

  vec3 Seek(const vec3& target);

  vec3 pos_, rot_ = vec3(0, 0 ,0), vel_;
  int death_timer_ = 100;
  int type_;
 private:
  map<string, AnimatedModel> models_;
  string current_model_ = "run";
  mat4 mat_;
  bool dead_ = false;
};

#endif
