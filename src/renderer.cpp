#include "renderer.h"

//public functions

Renderer::Renderer(uint32_t window_width, uint32_t window_height){

  //perspective projection
  projection = glm::perspective(
     90.0f,      //fov
     1.0f,//(float)window_width/(float)window_height1,  //Aspect ratio
     0.01f,       //near clipping plane
     64.0f      //far clipping plane
     );

  m_window_width = window_width;
  m_window_height = window_height;
  
  //view matrix
  view = glm::lookAt(
     glm::vec3(0, 0, 0), //camera position
     glm::vec3(0,0, 1),  //look at position
     glm::vec3(0,-1,0)  //up vector
     );

  model = glm::mat4(1.0f);
  mvp = projection * view;
}

bool Renderer::init(){
  if(SDL_Init(SDL_INIT_VIDEO)<0){
    printf("Could not init SDL!\n");
    return false;
  }
  main_window = SDL_CreateWindow("Voxel",
				 SDL_WINDOWPOS_CENTERED,
				 SDL_WINDOWPOS_CENTERED,
				 m_window_width,
				 m_window_height,
				 SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);

  if(!main_window){
    check_SDL_error(40);
    return false;
  }
  set_gl_attributes();
  
  main_context = SDL_GL_CreateContext(main_window);
  if(main_context == NULL){
    printf("Could not get GLContext!\n");
    return false;
  }

  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_CaptureMouse(SDL_TRUE);
  
  SDL_GL_SetSwapInterval(1);
  
  glewExperimental = GL_TRUE;
  glewInit();

  set_gl_options();

  return true;
  
}

bool Renderer::set_up_shader(const std::string vertex, const std::string fragment){
  std::string shader_folder("shader/");
  
  if(!shader.init())
    return false;

  if (!shader.load_shader( (shader_folder + vertex).c_str(), GL_VERTEX_SHADER))
    return false;

  if (!shader.load_shader( (shader_folder + fragment).c_str(), GL_FRAGMENT_SHADER))
    return false;

  if (!shader.link_shaders())
    return false;

  return true;
  
}

void Renderer::render_start(){
  glClearColor(0.5, 0.5, 0.5, 1.0);
  glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
}

void Renderer::render_model(const Model *model){
  shader.use_program();
  model->render();
}

void Renderer::render_mesh(const Mesh *mesh){
  shader.use_program();
  mesh->render();
}

void Renderer::render_end(){
  SDL_GL_SwapWindow(main_window);
}

void Renderer::push_matrix(glm::mat4 mat){
  model_stack.push_back(mat);
  model *= mat;
  set_matrix();
}

void Renderer::pop_matrix(){
  model *= glm::inverse(model_stack.back());
  model_stack.pop_back();
  set_matrix();
}

void Renderer::set_matrix(){
  shader.use_program();
  shader.set_mvp_matrix(model, view, projection);

  glm::mat4 normal_mat = glm::transpose(glm::inverse(view * model));
  shader.set_normal_matrix(normal_mat);
}

void Renderer::update_view_matrix(float x, float y){
  glm::vec3 euler;
  glm::vec3 world_up(0.0f,-1.0f,0.0f);
  
  float yaw = glm::radians(x);
  float pitch = glm::radians(y);
  
  euler.x = cos(yaw) * cos(pitch);
  euler.y = sin(pitch);
  euler.z = sin(yaw) * cos(pitch);

  glm::vec3 front = glm::normalize(euler);
  glm::vec3 right = glm::normalize(glm::cross(front, world_up));
  glm::vec3 up = glm::normalize(glm::cross(right,front));
  
  m_front = front;
  m_up = up;
  m_right = right;
  //view matrix
  view = glm::lookAt(
     glm::vec3(0, 0, 0), //camera position
     front,  //look at position
     up  //up vector
     );
}

glm::vec3 Renderer::get_front(){
  return m_front;
}

glm::vec3 Renderer::get_up(){
  return m_up;
}

glm::vec3 Renderer::get_right(){
  return m_right;
}


void Renderer::set_light(glm::vec3 light_pos){
  shader.use_program();
  shader.set_light( light_pos );
}


bool Renderer::in_frustum(){
  glm::vec4 Pclip = projection * view * model * glm::vec4(m_front * glm::vec3(3.0f), 1.0f);
  return (glm::abs(Pclip.x) < Pclip.w &&
	  glm::abs(Pclip.y) < Pclip.w &&
	  0 < Pclip.z &&
	  Pclip.z < Pclip.w);			   
}

void Renderer::cleanup(){
  model_stack.clear();
  model_stack.shrink_to_fit();
  
  shader.cleanup();

  SDL_GL_DeleteContext(main_context);

  SDL_DestroyWindow(main_window);
  
  SDL_Quit();
}

void Renderer::check_SDL_error(int line = -1){
  
  std::string error = SDL_GetError();
  if(error != ""){
    std::cout << "SDL Error: " << error << std::endl;

    if(line != -1)
      std::cout << "\nLine : " << line << std::endl;

    SDL_ClearError();
  }

}
  
//private functions

void Renderer::set_gl_options(){
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //setting up depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  //  glEnable(GL_CULL_FACE);

}

bool Renderer::set_gl_attributes(){

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  return true;
}
