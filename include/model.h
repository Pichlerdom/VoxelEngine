#ifndef _MODEL_H_
#define _MODEL_H_
 
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "shader.h"

#define POS_ATTR_INDEX 0
#define COLOR_ATTR_INDEX 1


class Model{
 public:
  Model();
  //returns the model matrix
  glm::mat4 get_model();
  
  //sets the model matrix uniform
  void set_matrix(glm::mat4 mvp);

  //renders the model using the shader set by set_shader(Shader _shader);
  void render() const;

  //sets the model matrix to the identity matrix
  void reset_matrix();

  //translate, rotate or scale the model
  void translate(const glm::vec3 &axis);
  void rotate();
  void scale();

  //initalizes the gl buffers with data from the file in ./objects/object_name/*
  bool setup_buffer_objects(const std::string object_name);

  void cleanup();

  std::vector<GLfloat> read_file(const char* file);

  void set_shader(Shader _shader);
  
 private:
  std::vector<GLfloat> positions;
  std::vector<GLfloat> colors;

  GLuint vbo[2], vao[1];

  glm::mat4 model;

  Shader shader;
  
};

#endif
