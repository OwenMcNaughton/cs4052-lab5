#include "Boid.hh"

#include <cmath>
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>
#include "Util.hh"

float kSepDist = 1.0f;
float kPredatorSepDist = 3.0f;
float kCohDist = 2.0f;
float kAliDist = 5.0f;
float kObsDist = 2.0f;
float kKillDist = 2.5f;

float kSepFac = 0.1f;
float kPredatorSepFac = 1.0f;
float kCohFac = 0.03f;
float kAliFac = 0.16f;
float kObsFac = 0.1f;
float kCenFac = 0.0001f;

float kMaxSpeed = 0.3f;
float kMaxForce = 0.1f;

vector<string> rabbit_run_models = {
  "rabbit1", "rabbit2", "rabbit3", "rabbit4", "rabbit3", "rabbit2",
  "rabbit1", "rabbit5", "rabbit6", "rabbit7", "rabbit6", "rabbit5"
};
vector<string> fox_run_models = {
  "fox1", "fox2", "fox3", "fox4", "fox3", "fox2",
  "fox1", "fox5", "fox6", "fox7", "fox6", "fox5"
};

vector<int> running_ticks = {
  20, 30, 60, 100, 60, 30, 20, 30, 60, 100, 60, 30
};

void PrintVec(const vec3& vec) {
  cout << vec.x << " " << vec.y << " " << vec.z << endl;
}

float Angle(float x, float z) {
  return float(-atan2(x, -z)) - 3.1416;
}

void Limit(vec3& vec, float max) {
  if (length(vec) > max) {
    normalize(vec);
    vec *= max;
  }
}

Boid::Boid(const mat4& mat, int type)
    : mat_(mat), type_(type) {
  vec3 scale, skew;
  quat ori;
  vec4 persp;
  decompose(mat_, scale, ori, pos_, skew, persp);

  if (type == kRabbit) {
    AnimatedModel rabbit_run(rabbit_run_models, "rabbit", mat, running_ticks, true);
    models_.emplace("run", rabbit_run);
  }

  if (type == kFox) {
    AnimatedModel fox_run(fox_run_models, "fox", mat, running_ticks);
    models_.emplace("run", fox_run);
  }
}

vec3 Boid::Seek(const vec3& vec) {
  vec3 result = vec;
  result -= pos_;
  result -= vel_;
  if (Util::mode_ == Util::kBoidsMode) {
    Limit(result, kMaxForce);
  } else {
    Limit(result, type_ == kFox ? kMaxForce * 10 : kMaxForce);
  }
  return result;
}

bool Boid::Update(int dt) {
  if (death_timer_ < 100) {
    death_timer_--;
  } else {
    dt = dt * length(vel_) * 4;
    models_.at(current_model_).Update(dt);
  }

  if (death_timer_ == 100 && type_ != kFox &&
      (pos_.x < -100 || pos_.x > 100 || pos_.z < -100 || pos_.z > 100)) {
    death_timer_ = 99;
  }

  if (death_timer_ <= 0) {
    return true;
  }

  return false;
}

void Boid::Render(const mat4& root, GLuint shader_program) {
  mat4 local(1.0f);
  local = translate(local, pos_);
  if (death_timer_ < 100) {
    local = rotate(local, float(radians(90.0f)), vec3(0, 0, 1));
  } else {
    float angle = Angle(vel_.x, vel_.z);
    local = rotate(local, angle, vec3(0, 1, 0));
  }

  vec3 scale, skew, pos;
  quat ori;
  vec4 persp;
  decompose(mat_, scale, ori, pos, skew, persp);
  local = translate(local, -pos);

  glUniform1f(glGetUniformLocation(shader_program, "alpha"), death_timer_ / 100.0f);
  models_.at(current_model_).Render(local, shader_program);
  glUniform1f(glGetUniformLocation(shader_program, "alpha"), 1.0f);
}

void Boid::Flock(
    int dt, const vector<Boid>& prey, const vector<Boid>& predators,
    const vector<vec3>& obstacles, const vec3& center) {
  if (death_timer_ < 100) {
    return;
  }

  vec3 acc(0.0f);

  acc += type_ == kRabbit
    ? Separation(prey, kSepDist) * kSepFac
    : vec3(0.0f);

  acc += Separation(predators, kPredatorSepDist) * kPredatorSepFac;

  acc += Cohesion(prey, type_ == kRabbit ? kCohDist : kCohDist * 3) * kCohFac;

  acc += type_ == kRabbit
    ? Alignment(prey) * kAliFac
    : vec3(0.0f);

  acc += Obstacles(obstacles);


  acc += Seek(center) * (type_ == kRabbit && Util::mode_ != Util::kBoidsMode
      ? kCenFac * sqrt(float(Util::rabbit_count_ + 1 - prey.size()))
      : kCenFac);

  if (type_ == kFox && Util::mode_ != Util::kBoidsMode) {
    acc *= sqrt(sqrt(sqrt(float(Util::rabbit_count_ + 1 - prey.size()))));
  }

  vel_ += acc * (dt / 10.0f);
  if (Util::mode_ == Util::kBoidsMode) {
    Limit(vel_, kMaxSpeed);
  } else {
    Limit(vel_, type_ == kFox ? kMaxSpeed * 3 : kMaxSpeed);
  }
  vel_.y = 0;
  pos_ += vel_;
}

vec3 Boid::Separation(const vector<Boid>& others, float min_dist) {
  int count = 0;
  vec3 sum(0.0f, 0.0f, 0.0f);

  for (const Boid& other : others) {
    if (this == &other) continue;
    float dist = distance(pos_, other.pos_);
    if (dist < min_dist) {
      vec3 diff(pos_.x, pos_.y, pos_.z);
      diff -= other.pos_;
      normalize(diff);
      diff /= dist;
      sum += diff;
      count++;
    }

    if (dist < kKillDist && other.type_ == kFox && type_ != kFox &&
        Util::mode_ != Util::kBoidsMode) {
      death_timer_--;
    }
  }

  if (count > 0) {
    sum /= (float)count;
  }

  if (sum.x == 0 && sum.y == 0 && sum.z == 0) {
    return sum;
  }

  if (length(sum) > 0) {
    sum -= vel_;

    Limit(sum, kMaxForce);
  }

  return sum;
}

vec3 Boid::Obstacles(const vector<vec3>& obstacles) {
  int count = 0;
  vec3 sum(0.0f, 0.0f, 0.0f);

  for (const vec3& obstacle : obstacles) {
    float dist = distance(pos_, obstacle);
    if (dist < kObsDist) {
      vec3 diff(pos_.x, pos_.y, pos_.z);
      diff -= obstacle;
      normalize(diff);
      diff /= dist;
      sum += diff;
      count++;
    }
  }

  if (count > 0) {
    sum /= (float)count;
  }

  if (sum.x == 0 && sum.y == 0 && sum.z == 0) {
    return sum;
  }

  if (length(sum) > 0) {
    sum -= vel_;

    Limit(sum, kMaxForce);
  }

  return sum;
}

vec3 Boid::Cohesion(const vector<Boid>& others, float min_dist) {
  int count = 0;
  vec3 sum(0.0f, 0.0f, 0.0f);

  for (const Boid& other : others) {
    if (this == &other) continue;
    float dist = distance(pos_, other.pos_);
    if (dist < min_dist) {
      sum += other.pos_;
      count++;
    }
  }

  if (count > 0) {
    sum /= float(count);
    sum = Seek(sum);
    return sum;
  }

  return sum;
}

vec3 Boid::Alignment(const vector<Boid>& others) {
  int count = 0;
  vec3 sum(0.0f, 0.0f, 0.0f);

  for (const Boid& other : others) {
    if (this == &other) continue;
    float dist = distance(pos_, other.pos_);
    if (dist < kAliDist) {
      sum += other.vel_;
      count++;
    }
  }

  if (count > 0) {
    sum /= float(count);
    sum -= vel_;
    Limit(sum, kMaxForce);
    return sum;
  }

  return sum;
}
