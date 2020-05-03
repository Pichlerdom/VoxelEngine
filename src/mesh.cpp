#include "mesh.h"

Mesh::Mesh(){
  num_triangles = 0;
  m_was_finished = false;
  
  positions = new std::vector<GLfloat>();
  colors = new std::vector<GLfloat>();
  indices = new std::vector<GLushort>();
  normals = new std::vector<GLfloat>();
}

Mesh::~Mesh(){

  clear_gl_buffers();
  delete positions;
  delete colors;
  delete indices;
  delete normals;
}

int Mesh::get_num_triangles(){
  return num_triangles;
}

int Mesh::get_num_vertices(){
  return positions->size()/3;
}

void Mesh::render() const{
  if(!m_was_finished)
    return;
  
  glBindVertexArray(vao[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo[0]);

  glDrawElements(GL_TRIANGLES, num_triangles * 3,
		 GL_UNSIGNED_SHORT, (void*)0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
  glBindVertexArray(0);
}

unsigned int Mesh::add_vertex(GLfloat x, GLfloat y, GLfloat z){
  positions->push_back(x);
  positions->push_back(y);
  positions->push_back(z);
  
  return (positions->size()/3)-1;
}

void Mesh::add_triangle(int v1, int v2, int v3){ 
  indices->push_back(v1);
  indices->push_back(v2);
  indices->push_back(v3);
  num_triangles++;
}

void Mesh::add_normal(GLfloat x, GLfloat y, GLfloat z){
  normals->push_back(x);
  normals->push_back(y);
  normals->push_back(z);
}

unsigned int Mesh::add_color(GLfloat r, GLfloat g, GLfloat b, GLfloat alpha){
  colors->push_back(r);
  colors->push_back(g);
  colors->push_back(b);
  colors->push_back(alpha);
  return (colors->size()/4)-1;
}


void Mesh::finish_mesh(){
  if(get_num_vertices() <= 0 ||
     get_num_triangles() <= 0){
    positions->clear();
    positions->shrink_to_fit();
    colors->clear();
    colors->shrink_to_fit();
    indices->clear();
    indices->shrink_to_fit();
    normals->clear();
    normals->shrink_to_fit();
    return;
  }

  if((colors->size()/4) != (positions->size()/3)){
    printf("Mesh was shit!!!\n\t%ld positions\n\t%ld colors\n\n",
	   positions->size()/3,
	   colors->size()/4);
    exit(2);
  }

  
  glGenVertexArrays(1, vao);
  glBindVertexArray(vao[0]);

  glGenBuffers(3, vbo);

  //Positions
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER,
	       positions->size() * sizeof(GLfloat),
	       positions->data(), GL_STATIC_DRAW);

  glVertexAttribPointer(POS_ATTR_MESH, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(POS_ATTR_MESH);

  //Color 
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER,
	       colors->size() * sizeof(GLfloat),
	       colors->data(), GL_STATIC_DRAW);
  
  glVertexAttribPointer(COLOR_ATTR_MESH, 4, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(COLOR_ATTR_MESH);

  //Normals
  glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
  glBufferData(GL_ARRAY_BUFFER,
	       normals->size() * sizeof(GLfloat),
	       normals->data(), GL_STATIC_DRAW);
  glVertexAttribPointer(NORMAL_ATTR_MESH, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(NORMAL_ATTR_MESH);
  
  //Indices
  glGenBuffers(1, ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	       indices->size() * sizeof(GLushort),
	       indices->data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);  

  
  m_was_finished = true;

  positions->clear();
  positions->shrink_to_fit();
  colors->clear();
  colors->shrink_to_fit();
  indices->clear();
  indices->shrink_to_fit();
  normals->clear();
  normals->shrink_to_fit();
}

bool Mesh::was_finished(){
  return m_was_finished;
}

void Mesh::reset(){
  
  delete positions;
  delete colors;
  delete indices;
  delete normals;

  positions = new std::vector<GLfloat>();
  colors = new std::vector<GLfloat>();
  indices = new std::vector<GLushort>();
  normals = new std::vector<GLfloat>();

  clear_gl_buffers();
  m_was_finished = false;
  num_triangles = 0;
}

void Mesh::clear_gl_buffers(){
  if(m_was_finished){
    glDisableVertexAttribArray(vao[0]);
    
    glDeleteBuffers(3,vbo);
    
    glDeleteBuffers(1,ibo);
    glDeleteVertexArrays(1,vao);
  }
}

