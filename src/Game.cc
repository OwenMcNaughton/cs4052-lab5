#include "Game.hh"

#include "Assets.hh"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
#include "Text.hh"
#include "Util.hh"

Game::Game(SDL_Window* window) {
  TextInit("holstein");
  window_ = window;
  shader_program_ = Assets::A().GetShader("standard");
  view_id_ = glGetUniformLocation(shader_program_, "view");
  proj_id_ = glGetUniformLocation(shader_program_, "proj");
  sampler_id_ = glGetUniformLocation(shader_program_, "sampler");
  light_id_ = glGetUniformLocation(shader_program_, "light_pos");

  for (int i = 0; i != 20 && Util::mode_ != Util::kBoidsMode; i++) {
    mat4 mat(1.0f);
    Tree* tree = new Tree(mat, 7);
    mat = translate(mat, vec3(rand() % 100 - 50.0f, 0.0f, rand() % 100 - 50.0f));
    tree->mat_ = mat;
    forest_.push_back(tree);
  }

  rabbit_center_ = vec3(0, 0, 0);

  mat4 center = translate(mat4(1.0f), rabbit_center_);

  mat4 lower_ground = translate(root_, vec3(0, -.2, 0));
  Model ground("ground", "ground", lower_ground);
  other_models_.push_back(ground);

  mat4 skymat = translate(mat4(1.0f), vec3(0, 5, 0));
  Model sky("skycube", "skycube", skymat, false, 10.0f);
  other_models_.push_back(sky);

  Util::rabbit_count_ = Util::mode_ == Util::kBoidsMode ? 500 : 300;
  for (int i = 0; i != Util::rabbit_count_; i++) {
    mat4 mat(1.0f);
    mat = translate(mat, vec3(0, -.1, 0));
    mat = translate(mat, vec3(rand() % 100 - 50.0f, 0.0f, rand() % 100 - 50.0f));
    Boid rabbit(mat, kRabbit);
    rabbits_.push_back(rabbit);
  }

  for (int i = 0; i != 10; i++) {
    mat4 mat(1.0f);
    mat = translate(mat, vec3(0, -0.14, 0));
    mat = translate(mat, vec3(rand() % 300 - 150.0f, 0.0f, rand() % 300 - 150.0f));
    Boid fox(mat, kFox);
    foxes_.push_back(fox);
  }

  mat4 mm(1.0f);
  mm = translate(mm, vec3(2, 3, 0.2));
  Model reticule("reticule", "", mm);
  overlay_models_.push_back(reticule);

  mat4 mn(1.0f);
  mn = translate(mn, vec3(2, 2.9, 0.1));
  mn = rotate(mn, -0.3f, vec3(1, 0, 0));
  Model minigun("minigun", "minigun", mn);
  overlay_models_.push_back(minigun);

  last_tick_ = SDL_GetTicks();

  light_pos_ = vec3(0, 50, 0);

  root_ = mat4(1.0f);

  overlay_cam_.Update(1);

  glUseProgram(shader_program_);
}

void Game::Start() {
  bool run = true;
  SDL_SetRelativeMouseMode(SDL_TRUE);
  while (run) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        run = false;
        break;
      }
    }
    Update();
    Render();
    SDL_PumpEvents();
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
      RayPick();
    }
  }
}

void Game::RayPick() {
  vec3 cam = cam_.GetPos();
  vec3 dir = cam_.GetHeading();
  float dir_len = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
  vec3 nv(dir.x / dir_len, dir.y / dir_len, dir.z / dir_len);
  for (Boid& rabbit : rabbits_) {
    vec3 sphere = rabbit.pos_;
    vec3 a(sphere.x - cam.x, sphere.y - cam.y, sphere.z - cam.z);
    vec3 a2(a.y * nv.z - a.z * nv.y, a.z * nv.x - a.x * nv.z, a.x * nv.y - a.y * nv.x);
    float a2pow = a2.x * a2.x + a2.y * a2.y + a2.z * a2.z;
    if (a2pow < 1) {
      rabbit.death_timer_ = 99;
    }
  }
  for (Boid& fox : foxes_) {
    vec3 sphere = fox.pos_ + vec3(0, 1, 0);
    vec3 a(sphere.x - cam.x, sphere.y - cam.y, sphere.z - cam.z);
    vec3 a2(a.y * nv.z - a.z * nv.y, a.z * nv.x - a.x * nv.z, a.x * nv.y - a.y * nv.x);
    float a2pow = a2.x * a2.x + a2.y * a2.y + a2.z * a2.z;
    if (a2pow < 3) {
      fox.death_timer_ = 99;
    }
  }
}

void Game::Update() {
  int dt = SDL_GetTicks() - last_tick_;
  last_tick_ = SDL_GetTicks();
  cout << dt << endl;

  cam_.Update(dt);

  float delta = dt / 20.0f;
  vec3 scale, skew, pos;
  quat ori;
  vec4 persp;
  decompose(other_models_[0].mat_, scale, ori, pos, skew, persp);

  rabbit_center_ = pos;

  vector<vec3> tree_positions;
  for (Tree* tree : forest_) {
    tree->Update(dt);
    vec3 scale, skew, pos;
    quat ori;
    vec4 persp;
    decompose(tree->mat_, scale, ori, pos, skew, persp);
    tree_positions.push_back(pos);
  }

  for (int i = 0; i != rabbits_.size(); i++) {
    bool killed = rabbits_[i].Update(dt);
    if (killed) {
      rabbits_.erase(rabbits_.begin() + i--);
    }

    rabbits_[i].Flock(dt, rabbits_, foxes_, tree_positions, rabbit_center_);
  }

  for (Boid& fox : foxes_) {
    fox.Update(dt);
    fox.Flock(dt, rabbits_, foxes_, tree_positions, rabbit_center_);
  }

  SDL_PumpEvents();
  if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
    overlay_models_[1].mat_ = rotate(overlay_models_[1].mat_, 0.1f, vec3(0, 0, 1));
  }
}

void Game::Render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shader_program_);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUniformMatrix4fv(view_id_, 1, GL_FALSE, value_ptr(cam_.GetView()));
  glUniformMatrix4fv(proj_id_, 1, GL_FALSE, value_ptr(cam_.GetProj()));

  glUniform3f(light_id_, light_pos_.x, light_pos_.y, light_pos_.z);

  mat4 smaller_trees = translate(root_, vec3(0, 3, 0));
  for (Tree* tree: forest_) {
    tree->Render(smaller_trees, shader_program_);
  }

  if (Util::mode_ != Util::kBoidsMode) {
    for (Model& other_model_ : other_models_) {
      other_model_.Render(root_, shader_program_);
    }
  }

  int living_rabbits = 0;
  for (Boid& rabbit : rabbits_) {
    rabbit.Render(root_, shader_program_);
    living_rabbits += rabbit.death_timer_ == 100 ? 1 : 0;
  }

  int living_foxes = 0;
  for (Boid& fox : foxes_) {
    fox.Render(root_, shader_program_);
    living_foxes += fox.death_timer_ == 100 ? 1 : 0;
  }

  glDisable(GL_BLEND);

  glUniformMatrix4fv(view_id_, 1, GL_FALSE, value_ptr(overlay_cam_.GetView()));
  glUniformMatrix4fv(proj_id_, 1, GL_FALSE, value_ptr(overlay_cam_.GetProj()));

  for (Model& overlay_model : overlay_models_) {
    overlay_model.Render(root_, shader_program_);
  }

  PrintText(to_string(living_rabbits) + " rabbits left", 0, 0, 24);
  PrintText(to_string(living_foxes) + " foxes left", 0, 30, 24);

  if (living_foxes == 0 && living_rabbits > 20) {
    PrintText("You win!", 200, 400, 40);
    PrintText("Score: " + to_string(living_rabbits) + " rabbits", 60, 200, 40);
  }

  if (living_rabbits < 20) {
    PrintText("You lose!", 200, 400, 40);
  }

  SDL_GL_SwapWindow(window_);
}
