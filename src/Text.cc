#include "Text.hh"

#include "Assets.hh"
#include <cstring>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

using namespace glm;
using namespace std;

unsigned int Text2DTextureID;
unsigned int Text2DVertexBufferID;
unsigned int Text2DUVBufferID;
unsigned int Text2DShaderID;
unsigned int Text2DUniformID;

void TextInit(const string& font) {
  Text2DTextureID = Assets::A().GetTexture("holstein");

  glGenBuffers(1, &Text2DVertexBufferID);
  glGenBuffers(1, &Text2DUVBufferID);

  Text2DShaderID = Assets::A().GetShader("text");

  Text2DUniformID = glGetUniformLocation( Text2DShaderID, "myTextureSampler" );
}

void PrintText(const string& text, int x, int y, int size){
  int length = strlen(text.c_str());

  vector<vec2> vertices;
  vector<vec2> UVs;
  for (int i = 0 ; i < length ; i++) {
    vec2 vertex_up_left = vec2(x + i * size, y + size);
    vec2 vertex_up_right = vec2(x + i * size + size, y + size);
    vec2 vertex_down_right = vec2(x + i * size + size, y);
    vec2 vertex_down_left = vec2(x + i * size, y);

    vertices.push_back(vertex_up_left   );
    vertices.push_back(vertex_down_left );
    vertices.push_back(vertex_up_right  );

    vertices.push_back(vertex_down_right);
    vertices.push_back(vertex_up_right);
    vertices.push_back(vertex_down_left);

    char character = text.c_str()[i];
    float uv_x = (character%16)/16.0f;
    float uv_y = (character/16)/16.0f;

    vec2 uv_up_left = vec2(uv_x, uv_y);
    vec2 uv_up_right = vec2(uv_x + 1.0f / 16.0f, uv_y);
    vec2 uv_down_right = vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
    vec2 uv_down_left = vec2(uv_x, (uv_y + 1.0f/16.0f));
    UVs.push_back(uv_up_left);
    UVs.push_back(uv_down_left);
    UVs.push_back(uv_up_right);

    UVs.push_back(uv_down_right);
    UVs.push_back(uv_up_right);
    UVs.push_back(uv_down_left);
  }
  glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), &vertices[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
  glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(vec2), &UVs[0], GL_STATIC_DRAW);

  glUseProgram(Text2DShaderID);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, Text2DTextureID);
  glUniform1i(Text2DUniformID, 0);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDrawArrays(GL_TRIANGLES, 0, vertices.size() );

  glDisable(GL_BLEND);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}
