#include "Model.hh"

#include "Assets.hh"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Model::Model(
  const string& mesh,
  const string& texture,
  const mat4& mat,
  bool advanced_render
) {
  vert_buffer_ = Assets::A().GetVertBuffer(mesh);
  uv_buffer_ = Assets::A().GetUVBuffer(mesh);
  norm_buffer_ = Assets::A().GetNormBuffer(mesh);
  texture_ = Assets::A().GetTexture(texture);
  specmap_ = Assets::A().GetTexture(texture + "_spec");
  vert_count_ = Assets::A().GetVertCount(mesh);
  mat_ = mat;
  mesh_ = mesh;
  advanced_render_ = advanced_render;
}

void Model::Update(int dt) {
  if (update_func_) {
    this->update_func_(dt);
  }

  for (auto& child : children_) {
    child->Update(dt);
  }
}

void Model::Render(const mat4& root, GLuint shader_program) {
  if (advanced_render_) {
    glUniform1i(glGetUniformLocation(shader_program, "advanced"), 1);
  } else {
    glUniform1i(glGetUniformLocation(shader_program, "advanced"), 0);
  }

  mat4 local = root * mat_;
  glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, value_ptr(local));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glUniform1i(glGetUniformLocation(shader_program, "sampler"), 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, specmap_);
  glUniform1i(glGetUniformLocation(shader_program, "spec_sampler"), 1);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vert_buffer_);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer_);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, norm_buffer_);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glDrawArrays(GL_TRIANGLES, 0, vert_count_);

  for (auto& child : children_) {
    child->Render(local, shader_program);
  }
}

void Model::AddChild(Model* child) {
  children_.push_back(child);
}

AnimatedModel::AnimatedModel(
    const vector<string>& meshes,
    const string& texture,
    const mat4& mat,
    const vector<int>& tick_counts,
    bool advanced_render)
    : Model("", "", mat, advanced_render), tick_counts_(tick_counts) {
  for (const string& mesh: meshes) {
    models_.push_back(Model(mesh, texture, mat, advanced_render));
  }
  vert_buffer_ = models_[model_idx_].vert_buffer_;
  uv_buffer_ = models_[model_idx_].uv_buffer_;
  norm_buffer_ = models_[model_idx_].norm_buffer_;
  texture_ = models_[model_idx_].texture_;
}

void AnimatedModel::Update(int dt) {
  elapsed_ticks_ += dt;
  if (elapsed_ticks_ > tick_counts_[model_idx_]) {
    elapsed_ticks_ = 0;
    model_idx_++;
    if (model_idx_ == models_.size()) {
      model_idx_ = 0;
    }
  }
}

void AnimatedModel::Render(const mat4& root, GLuint shader_program) {
  models_[model_idx_].Render(root, shader_program);
}
