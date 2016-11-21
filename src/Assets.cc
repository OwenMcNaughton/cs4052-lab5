#include "Assets.hh"

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

Assets::Assets() {
  LoadObjFile("assets/leaves.obj", "leaves");
  LoadObjFile("assets/trunk.obj", "trunk");
  LoadObjFile("assets/ground.obj", "ground");

  LoadObjFile("assets/rabbit1.obj", "rabbit1");
  LoadObjFile("assets/rabbit2.obj", "rabbit2");
  LoadObjFile("assets/rabbit3.obj", "rabbit3");
  LoadObjFile("assets/rabbit4.obj", "rabbit4");
  LoadObjFile("assets/rabbit5.obj", "rabbit5");
  LoadObjFile("assets/rabbit6.obj", "rabbit6");
  LoadObjFile("assets/rabbit7.obj", "rabbit7");

  LoadObjFile("assets/fox1.obj", "fox1");
  LoadObjFile("assets/fox2.obj", "fox2");
  LoadObjFile("assets/fox3.obj", "fox3");
  LoadObjFile("assets/fox4.obj", "fox4");
  LoadObjFile("assets/fox5.obj", "fox5");
  LoadObjFile("assets/fox6.obj", "fox6");
  LoadObjFile("assets/fox7.obj", "fox7");
  LoadObjFile("assets/fox.obj", "fox");

  LoadObjFile("assets/minigun.obj", "minigun");
  LoadObjFile("assets/reticule.obj", "reticule");

  LoadObjFile("assets/skycube.obj", "skycube");
  LoadBitmap("assets/skycube.bmp", "skycube");

  LoadBitmap("assets/leaves256.bmp", "leaves256");
  LoadBitmap("assets/trunk.bmp", "trunk");
  LoadBitmap("assets/ground.bmp", "ground");
  LoadBitmap("assets/rabbit.bmp", "rabbit");
  LoadBitmap("assets/rabbit_spec.bmp", "rabbit_spec");
  LoadBitmap("assets/fox.bmp", "fox");
  LoadBitmap("assets/minigun.bmp", "minigun");

  LoadDDS("assets/holstein.dds", "holstein");

  CompileShaders("vertexShader", "fragmentShader", "standard");
  CompileShaders("textVert", "textFrag", "text");
}

void Assets::LoadObjFile(const string& path, const string& id) {
  vector<int> vert_indices, uv_indices, norm_indices;
  vector<vec3> temp_verts;
  vector<vec2> temp_uvs;
  vector<vec3> temp_norms;

  FILE* file = fopen(path.c_str(), "r");
  for (;;) {
    char line_header[128];
    int res = fscanf(file, "%s", line_header);
    if (res == EOF) {
      break;
    }

    if (strcmp(line_header, "v") == 0 ){
      vec3 vert;
      fscanf(file, "%f %f %f\n", &vert.x, &vert.y, &vert.z);
      temp_verts.push_back(vert);
    } else if (strcmp(line_header, "vt") == 0) {
      vec2 uv;
      fscanf(file, "%f %f\n", &uv.x, &uv.y );
      temp_uvs.push_back(uv);
    } else if (strcmp(line_header, "vn") == 0) {
      vec3 norm;
      fscanf(file, "%f %f %f\n", &norm.x, &norm.y, &norm.z);
      temp_norms.push_back(norm);
    } else if (strcmp(line_header, "f") == 0) {
      string vert1, vert2, vert3;
      int vert_index[3], uv_index[3], norm_index[3];
      int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
        &vert_index[0], &uv_index[0], &norm_index[0],
        &vert_index[1], &uv_index[1], &norm_index[1],
        &vert_index[2], &uv_index[2], &norm_index[2]);
      vert_indices.push_back(vert_index[0]);
      vert_indices.push_back(vert_index[1]);
      vert_indices.push_back(vert_index[2]);
      uv_indices.push_back(uv_index[0]);
      uv_indices.push_back(uv_index[1]);
      uv_indices.push_back(uv_index[2]);
      norm_indices.push_back(norm_index[0]);
      norm_indices.push_back(norm_index[1]);
      norm_indices.push_back(norm_index[2]);
    } else {
      char comment_buffer[1000];
      fgets(comment_buffer, 1000, file);
    }
  }

  vector<vec3> verts;
  vector<vec2> uvs;
  vector<vec3> norms;
  for (int i = 0; i < vert_indices.size(); i++) {
    int vert_index = vert_indices[i];
    int uv_index = uv_indices[i];
    int norm_index = norm_indices[i];

    verts.push_back(temp_verts[vert_index - 1]);
    uvs.push_back(temp_uvs[uv_index - 1]);
    norms.push_back(temp_norms[norm_index - 1]);
  }
  fclose(file);

  GLuint vert_buffer, uv_buffer, norm_buffer, vert_count;
  glGenBuffers(1, &vert_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vert_buffer);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vec3), &verts[0], GL_STATIC_DRAW);

  glGenBuffers(1, &uv_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
  glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);

  glGenBuffers(1, &norm_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, norm_buffer);
  glBufferData(GL_ARRAY_BUFFER, norms.size() * sizeof(vec3), &norms[0], GL_STATIC_DRAW);

  vert_counts_.emplace(id, uvs.size());
  vert_buffers_.emplace(id, vert_buffer);
  uv_buffers_.emplace(id, uv_buffer);
  norm_buffers_.emplace(id, norm_buffer);
}

void Assets::LoadBitmap(const string& path, const string& id) {
  char header[54];
  int dataPos;
  int imageSize;
  int width, height;
  char* data;

  FILE * file = fopen(path.c_str(), "rb");
  fread(header, 1, 54, file);

  dataPos = *(int*)&(header[0x0A]);
  imageSize = *(int*)&(header[0x22]);
  width = *(int*)&(header[0x12]);
  height = *(int*)&(header[0x16]);

  if (imageSize == 0) {
    imageSize = width * height * 3;
  }
  if (dataPos == 0) {
    dataPos = 54;
  }

  data = new char[imageSize];
  fread(data, 1, imageSize, file);
  fclose(file);

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);

  textures_.emplace(id, texture);
}

int Assets::GetVertCount(const string& id) {
  return vert_counts_[id];
}

GLuint Assets::GetVertBuffer(const string& id) {
  return vert_buffers_[id];
}

GLuint Assets::GetUVBuffer(const string& id) {
  return uv_buffers_[id];
}

GLuint Assets::GetNormBuffer(const string& id) {
  return norm_buffers_[id];
}

GLuint Assets::GetTexture(const string& id) {
  return textures_[id];
}

string Assets::readShaderSource(const string& fileName) {
  ifstream file(fileName.c_str());
  if(file.fail()) {
    cout << "error loading shader called " << fileName;
    exit (1);
  }

  stringstream stream;
  stream << file.rdbuf();
  file.close();

  return stream.str();
}

void Assets::AddShader(GLuint sProgram, string sText, GLenum sType) {
  GLuint shaderObj = glCreateShader(sType);

  if (shaderObj == 0) {
    fprintf(stderr, "Error creating shader type %d\n", sType);
    exit(0);
  }
  string outShader = readShaderSource(sText.c_str());
  const char* pShaderSource = outShader.c_str();

  glShaderSource(shaderObj, 1, (const GLchar**)&pShaderSource, NULL);
  glCompileShader(shaderObj);
  GLint success;
  glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infolog[1024];
    glGetShaderInfoLog(shaderObj, 1024, NULL, infolog);
    cout << "Shader failure: " << infolog << endl;
    exit(1);
  }
  glAttachShader(sProgram, shaderObj);
}

void Assets::CompileShaders(const string& vert, const string& frag, const string& id) {
  GLuint shader_program = glCreateProgram();
  if (shader_program == 0) {
    fprintf(stderr, "Error creating shader program\n");
    exit(1);
  }

  AddShader(shader_program, "Shaders/" + vert + ".txt", GL_VERTEX_SHADER);
  AddShader(shader_program, "Shaders/" + frag + ".txt", GL_FRAGMENT_SHADER);

  GLint Success = 0;
  GLchar ErrorLog[1024] = { 0 };

  glLinkProgram(shader_program);
  glGetProgramiv(shader_program, GL_LINK_STATUS, &Success);
  if (Success == 0) {
    glGetProgramInfoLog(shader_program, sizeof(ErrorLog), NULL, ErrorLog);
    fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
    exit(1);
  }

  glValidateProgram(shader_program);
  glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &Success);
  if (!Success) {
    glGetProgramInfoLog(shader_program, sizeof(ErrorLog), NULL, ErrorLog);
    fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
    exit(1);
  }
  glUseProgram(shader_program);
  shaders_.emplace(id, shader_program);
}

GLuint Assets::GetShader(const string& id) {
  return shaders_[id];
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

void Assets::LoadDDS(const string& path, const string& id) {
  char header[124];
  FILE *fp;
  fp = fopen(path.c_str(), "rb");
  char filecode[4];
  fread(filecode, 1, 4, fp);
  if (strncmp(filecode, "DDS ", 4) != 0) {
    fclose(fp);
    return;
  }

  fread(&header, 124, 1, fp);

  int height      = *( int*)&(header[8 ]);
  int width	     = *( int*)&(header[12]);
  int linearSize	 = *( int*)&(header[16]);
  int mipMapCount = *( int*)&(header[24]);
  int fourCC      = *( int*)&(header[80]);


  char * buffer;
  int bufsize;
  bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
  buffer = ( char*)malloc(bufsize * sizeof( char));
  fread(buffer, 1, bufsize, fp);
  fclose(fp);

  int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
  int format;
  switch(fourCC) {
    case FOURCC_DXT1:
      format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
      break;
    case FOURCC_DXT3:
      format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
      break;
    case FOURCC_DXT5:
      format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
      break;
    default:
      free(buffer);
      return;
  }

  GLuint textureID;
  glGenTextures(1, &textureID);

  glBindTexture(GL_TEXTURE_2D, textureID);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);

  int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
  int offset = 0;

  for ( int level = 0; level < mipMapCount && (width || height); ++level) {
    int size = ((width+3)/4)*((height+3)/4)*blockSize;
    glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
      0, size, buffer + offset);

    offset += size;
    width  /= 2;
    height /= 2;

    if (width < 1) width = 1;
    if (height < 1) height = 1;
  }

  free(buffer);
  textures_.emplace(id, textureID);
}
