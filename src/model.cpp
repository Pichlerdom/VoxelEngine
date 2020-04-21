#include "model.h"

Model::Model(){
  reset_matrix();
}

glm::mat4 Model::get_model(){
  return model;
}
  
void Model::set_matrix(glm::mat4 mvp){
  shader.use_program();
  shader.set_matrix(mvp);
}

void Model::render() const {
  shader.use_program();
  glDrawArrays(GL_TRIANGLES, 0, positions.size() / 3);//GL_TRIANGLES
}

void Model::reset_matrix(){
  model = glm::mat4(1.0f);
}

void Model::translate(const glm::vec3 &axis){
  model *= glm::translate(axis);
}

void Model::rotate(){}
void Model::scale(){}

bool Model::setup_buffer_objects(const std::string object_name){
  
  const std::string object_folder ("objects/");
  const std::string positions_file ("/positions.txt");
  const std::string colors_file ("/colors.txt");
  
  positions = read_file( (object_folder + object_name + positions_file).c_str());
  colors = read_file( (object_folder + object_name + colors_file).c_str());

  //  positions.pop_back();
  //colors.pop_back();
  
  glGenBuffers(2,vbo);

  glGenVertexArrays(1,vao);
  glBindVertexArray(vao[0]);

  //Color
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), &colors[0], GL_STATIC_DRAW);
  
  glVertexAttribPointer(COLOR_ATTR_INDEX, 4, GL_FLOAT, GL_FALSE, 0, 0);

  //Positions
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(GLfloat), &positions[0], GL_STATIC_DRAW);

  glVertexAttribPointer(POS_ATTR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
   
  //
  glEnableVertexAttribArray(POS_ATTR_INDEX);  
  glEnableVertexAttribArray(COLOR_ATTR_INDEX);

  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(vao[0]);
  
  return true;
}
  
void Model::cleanup(){
  glDisableVertexAttribArray(0);

  glDeleteBuffers(2,vbo);

  glDeleteVertexArrays(1, vao);

  SDL_Quit();
}

std::vector<GLfloat> Model::read_file(const char* file){
  // Open file
  std::ifstream t(file);

  std::cout << "Reading : " << file << std::endl;
  
  if(!t)
    std::cout << "Could not find file!" << std::endl;

  std::vector<GLfloat> result;

  while (t.good()){
    std::string str;
    t >> str;
    
    GLfloat f = std::atof(str.c_str());
    result.push_back(f);
  }

  return result;
}

void Model::set_shader(Shader _shader){
  shader = _shader;
}
  
