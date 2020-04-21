#include "mesh.h"

Mesh::Mesh(){
  num_triangles = 0;
  m_was_finished = false;
  reset_matrix();
  
  positions = new std::vector<GLfloat>();
  colors = new std::vector<GLfloat>();
  indices = new std::vector<unsigned int>();
}

Mesh::~Mesh(){
  reset();
  delete positions;
  delete colors;
  delete indices;
}

int Mesh::get_num_triangles(){
  return num_triangles;
}

int Mesh::get_num_vertices(){
  return positions->size()/3;
}

glm::mat4 Mesh::get_model(){
  return model;
}

void Mesh::set_matrix(glm::mat4 mvp){
  shader.use_program();
  shader.set_matrix(mvp);
}

void Mesh::translate(const glm::vec3 &axis){
  model *= glm::translate(axis);
}

//sets the model matrix to the identity matrix
void Mesh::reset_matrix(){
  model = glm::mat4(1.0f);
}


void Mesh::render() const{
  shader.use_program();
  if(m_was_finished)
    glDrawElements(GL_TRIANGLES, indices->size(),
		   GL_UNSIGNED_INT, (void*)0);
}

unsigned int Mesh::add_vertex(GLfloat x, GLfloat y, GLfloat z){
  positions->push_back(x);
  positions->push_back(y);
  positions->push_back(z);
  return (positions->size()/3);
}

void Mesh::add_triangle(int v1, int v2, int v3){ 
  indices->push_back(v1);
  indices->push_back(v2);
  indices->push_back(v3);
  num_triangles++;
}

unsigned int Mesh::add_color(GLfloat r, GLfloat g, GLfloat b, GLfloat alpha){
  colors->push_back(r);
  colors->push_back(g);
  colors->push_back(b);
  colors->push_back(alpha);
  return (colors->size()/4);
}


void Mesh::finish_mesh(){
  if(get_num_vertices() <= 0 ||
     get_num_triangles() <= 0){
    return;
  }
  
  if((colors->size()/4) != (positions->size()/3)){
    printf("Mesh was shit!!!\n\t%ld postions\n\t%ld colors\n\n",
	   positions->size()/3,
	   colors->size()/4);
    exit(2);
  }

  m_was_finished = true;
  
  glGenBuffers(3, vbo);

  glGenVertexArrays(1, vao);
  glBindVertexArray(vao[0]);

  //Positions
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER,
	       positions->size() * sizeof(GLfloat),
	       positions->data(), vbo[0]);

  glVertexAttribPointer(POS_ATTR_MESH, 4, GL_FLOAT,GL_FALSE, 0, 0);

  //Color 
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER,
	       colors->size() * sizeof(GLfloat),
	       colors->data(), GL_STATIC_DRAW);
  
  glVertexAttribPointer(COLOR_ATTR_MESH, 4, GL_FLOAT,GL_FALSE, 0, 0);

  //INDICES
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
	       indices->size() * sizeof(unsigned int),
	       indices->data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(POS_ATTR_MESH);
  glEnableVertexAttribArray(COLOR_ATTR_MESH);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(vao[0]);


  positions->clear();
  positions->shrink_to_fit();

  colors->clear();
  colors->shrink_to_fit();
  
  indices->clear();
  indices->shrink_to_fit();
   
}

bool Mesh::was_finished(){
  return m_was_finished;
}

void Mesh::reset(){
  if(m_was_finished){
    glDisableVertexAttribArray(0);
    
    glDeleteBuffers(3,vbo);
    
    glDeleteVertexArrays(1,vao);
  }
  m_was_finished = false;
}


void Mesh::set_shader(Shader _shader){
  shader = _shader;
}
