#ifndef _MESH_H_
#define _MESH_H_

#include "shader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#define POS_ATTR_MESH 0
#define COLOR_ATTR_MESH 1 
#define NORMAL_ATTR_MESH 2


class Mesh{
public:
  Mesh();
  ~Mesh();
  
  int get_num_triangles();
  int get_num_vertices();
  
  glm::mat4 get_model();
  
  //sets the model matrix uniform
  void set_matrix(glm::mat4 mvp);

  void render() const;

  //sets the model matrix to the identity matrix
  void reset_matrix();

  //translate, rotate or scale the model
  void translate(const glm::vec3 &axis);
  
  unsigned int add_vertex(GLfloat x, GLfloat y, GLfloat z);

  void add_triangle(int v1, int v2, int v3);

  void add_normal(GLfloat x, GLfloat y, GLfloat z);
  
  unsigned int add_color(GLfloat r, GLfloat g, GLfloat b, GLfloat alpha);

  void finish_mesh();

  void reset();
  
  bool was_finished();

private:
  bool m_was_finished;
  int num_triangles;
  
  std::vector<GLfloat> *positions;
  std::vector<GLfloat> *colors;
  std::vector<GLfloat> *normals;
  std::vector<GLushort> *indices;
  
  GLuint vbo[3], vao[1], ibo[1];

  void clear_gl_buffers();  
};

#endif
