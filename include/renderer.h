#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <iostream>

#define GL3_PROTOTYPE 1
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include "shader.h"
#include "model.h"


class Renderer{
 public:
  Renderer(uint32_t window_width, uint32_t window_height);
  bool init();

  bool set_up_shader(const std::string vertex, const std::string fragment);
  
  void render_start();
  void render_model(const Model &model);
  void render_end();
  
  glm::mat4 get_matrix();
  void set_matrix(const glm::mat4 &model);
  void set_shader(Model &model);

  void cleanup();
  void check_SDL_error(int line);
  
 private:
  uint32_t m_window_width;
  uint32_t m_window_height;
  
  glm::mat4 projection;
  glm::mat4 view;

  SDL_Window *main_window;
  SDL_GLContext main_context;

  Shader shader;

  void set_gl_options();
  bool set_gl_attributes();
  
};



#endif
