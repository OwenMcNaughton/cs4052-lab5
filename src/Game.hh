#ifndef GAME_HH
#define GAME_HH

#include "Camera.hh"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Model.hh"
#include "Boid.hh"
#include <SDL2/SDL.h>
#include "Tree.hh"

using namespace std;

class Game {
 public:
  Game(SDL_Window* window);

  void Start();

  void Update();

  void Render();

  void RayPick();

 private:
  SDL_Window* window_;
  GLuint shader_program_, view_id_, proj_id_, sampler_id_, light_id_;
  Camera cam_, overlay_cam_;
  vector<Tree*> forest_;
  vector<Boid> rabbits_;
  vector<Boid> foxes_;
  vector<Model> other_models_;
  vector<Model> overlay_models_;
  int last_tick_;
  vec3 light_pos_;
  mat4 root_;
  vec3 rabbit_center_;

  float spec_fac_ = 1.0f, ambi_fac_ = 1.0f, diff_fac_ = 1.0f;
};

#endif
