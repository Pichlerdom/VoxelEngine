#include"shader.h"

Shader::Shader(){
  mvp_matrix_id_set = false;
  normal_matrix_id_set = false;
  light_id_set = false;
}
    
void Shader::use_program() const {
  // Load the shader into the rendering pipeline
  glUseProgram(shaderProgram);
}
  

bool Shader::init(){
  // Generate our shader. This is similar to glGenBuffers() and glGenVertexArray(), except that this returns the ID
  shaderProgram = glCreateProgram();
		
  return true;
}

void Shader::set_light(const glm::vec3 light_pos){
  if(!light_id_set){
    light_id = glGetUniformLocation(shaderProgram, "light_pos");
    light_id_set = true;
  }
  glUniform3f(light_id,
	       light_pos.x,
	       light_pos.y,
	       light_pos.z);
}

void Shader::set_mvp_matrix(const glm::mat4 model,
			    const glm::mat4 view,
			    const glm::mat4 projection){
  if(!mvp_matrix_id_set){
    model_matrix_id = glGetUniformLocation(shaderProgram, "model");
    view_matrix_id = glGetUniformLocation(shaderProgram, "view");
    projection_matrix_id = glGetUniformLocation(shaderProgram, "projection");
    mvp_matrix_id_set = true;
  }

  glUniformMatrix4fv(model_matrix_id, 1, GL_FALSE, &model[0][0]);
  glUniformMatrix4fv(view_matrix_id, 1, GL_FALSE, &view[0][0]);
  glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, &projection[0][0]);
  
}

void Shader::set_normal_matrix(const glm::mat4 mat){
  if(!normal_matrix_id_set){
    normal_matrix_id = glGetUniformLocation(shaderProgram, "normal_mat");
    normal_matrix_id_set = true;
  }

  glUniformMatrix4fv(normal_matrix_id, 1, GL_FALSE, &mat[0][0]);
}



bool Shader::load_shader(const std::string &filename, GLenum shader_type){
  std::cout << "Loading Shader: " << filename << std::endl;

  int shader_id = create_shader(filename, shader_type);

  if(try_compile_shader(shader_id)){
    glAttachShader(shaderProgram, shader_id);
    shader_ids.push_back(shader_id);

    return true;
  }

  return false;
}


bool Shader::link_shaders(){
  // Link. At this point, our shaders will be inspected/optized and the binary code generated
  // The binary code will then be uploaded to the GPU
  glLinkProgram(shaderProgram);
 
  // Verify that the linking succeeded
  int isLinked;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);

  if (!isLinked)
    print_shader_linking_error(shaderProgram);

  return isLinked != 0;
}


void Shader::cleanup(){
  /* Cleanup all the things we bound and allocated */
  glUseProgram(0);
  for (auto i: shader_ids)
    glDetachShader(shaderProgram, i);
  
  glDeleteProgram(shaderProgram);

  for (auto i: shader_ids)
    glDeleteShader(i);
}


//private functions

std::string Shader::read_file(const char* file){
  // Open file
  std::ifstream t(file);

  if(!t){
    printf("File not found!\n");
    return "";
  }

  // Read file into buffer
  std::stringstream buffer;
  buffer << t.rdbuf();
      
  // Make a std::string and fill it with the contents of buffer
  std::string fileContent = buffer.str();

  return fileContent;
}


bool Shader::try_compile_shader(int shader_id){
  // Compile the vertex shader
  glCompileShader(shader_id);
 
  int wasCompiled = 0;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &wasCompiled );
 
  if (wasCompiled == false){
    print_shader_compilation_error_info(shader_id);
    return false;
  }

  return true;
}


int Shader::create_shader(const std::string &filename, GLenum shader_type){
  std::string str = read_file(filename.c_str());
 
  // c_str() gives us a const char*, but we need a non-const one
  char* src = const_cast<char*>( str.c_str());
  int32_t size = str.length();
		
  // Create an empty vertex shader handle
  int shader_id = glCreateShader(shader_type);
  // Send the vertex shader source code to OpenGL
  glShaderSource(shader_id, 1, &src, &size);
   
  return shader_id;
}


void Shader::print_shader_linking_error(int32_t shaderId){
  std::cout << "=======================================\n";
  std::cout << "Shader linking failed : " << std::endl;

  // Find length of shader info log
  int maxLength;
  glGetProgramiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

  std::cout << "Info Length : " << maxLength << std::endl;

  // Get shader info log
  char* shaderProgramInfoLog = new char[maxLength];
  glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, shaderProgramInfoLog);

  std::cout << "Linker error message : " << shaderProgramInfoLog << std::endl;

  /* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
  /* In this simple program, we'll just leave */
  delete shaderProgramInfoLog;
  return;
}


void Shader::print_shader_compilation_error_info(int32_t shaderId){
  std::cout << "=======================================\n";
  std::cout << "Shader compilation failed : " << std::endl;

  // Find length of shader info log
  int maxLength;
  glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

  // Get shader info log
  char* shaderInfoLog = new char[maxLength];
  glGetShaderInfoLog(shaderId, maxLength, &maxLength, shaderInfoLog );
 
  // Print shader info log
  std::cout << "\tError info : " << shaderInfoLog << std::endl;
 
  std::cout << "=======================================\n\n";
  delete shaderInfoLog;
}
