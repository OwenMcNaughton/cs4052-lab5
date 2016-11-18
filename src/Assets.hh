#ifndef ASSETS_HH
#define ASSETS_HH

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include <vector>

using namespace glm;
using namespace std;

class Assets {
 public:
  static Assets& A() {
    static Assets *instance = new Assets();
    return *instance;
  }

  Assets();

  void LoadObjFile(const string& path, const string& id);

  void LoadBitmap(const string& path, const string& id);
  void LoadDDS(const string& path, const string& id);

  int GetVertCount(const string& id);

  GLuint GetVertBuffer(const string& id);

  GLuint GetUVBuffer(const string& id);

  GLuint GetNormBuffer(const string& id);

  GLuint GetTexture(const string& id);

  string readShaderSource(const string& fileName);

  void AddShader(GLuint sProgram, string sText, GLenum sType);

  void CompileShaders(const string& vert, const string& frag, const string& id);

  GLuint GetShader(const string& id);

 private:
  map<string, int> vert_counts_;
  map<string, GLuint> vert_buffers_;
  map<string, GLuint> uv_buffers_;
  map<string, GLuint> norm_buffers_;
  map<string, GLuint> textures_;
  map<string, GLuint> shaders_;
};

#endif
