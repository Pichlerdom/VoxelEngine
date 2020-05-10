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
#include <math.h>

#include "shader.h"
#include "model.h"
#include "mesh.h"

class Renderer{
 public:
  Renderer(uint32_t window_width,
	   uint32_t window_height);
  bool init();

  bool set_up_shader(const std::string vertex,
		     const std::string fragment);

  void update_view_matrix(float x, float y);
  
  void push_matrix(glm::mat4 mat);
  void pop_matrix();

  void set_light(glm::vec3 light_pos);
  
  void render_start();
  void render_model(const Model *model);
  void render_mesh(const Mesh *mesh);
  void render_end();
  
  bool in_frustum();
  
  void cleanup();
  void check_SDL_error(int line);
  
  glm::vec3 get_front();
  glm::vec3 get_up();
  glm::vec3 get_right();

private:
  uint32_t m_window_width;
  uint32_t m_window_height;

  glm::vec3 m_front;
  glm::vec3 m_up;
  glm::vec3 m_right;
  
  glm::mat4 projection;
  glm::mat4 view;
  glm::mat4 model;
  glm::mat4 mvp;

  std::vector<glm::mat4> model_stack;
  
  SDL_Window *main_window;
  SDL_GLContext main_context;

  Shader shader;
  
  void set_matrix();

  void set_gl_options();
  bool set_gl_attributes();
  
};



#endif
