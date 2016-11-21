#include "Game.hh"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdlib.h>
#include "Util.hh"

using namespace std;

SDL_Window* WindowInit() {
  srand(time(NULL));

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window* window = SDL_CreateWindow(
    "", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    Util::U().GetWidth(), Util::U().GetHeight(), SDL_WINDOW_OPENGL
  );

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GLContext glContext = SDL_GL_CreateContext(window);

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();

  glClearColor(0.52f, 0.81f, 0.96f, 0.0f);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);

  GLuint vert_id;
  glGenVertexArrays(1, &vert_id);
  glBindVertexArray(vert_id);

  return window;
}

int main (int argc, char** argv) {
  SDL_Window* window = WindowInit();

  if (argc > 1) {
    Util::mode_ = atoi(argv[1]);
  } else {
    Util::mode_ = Util::kNormalMode;
  }

  Game game(window);
  game.Start();

  SDL_Quit();

  return 0;
}
