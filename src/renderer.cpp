#include "renderer.h"

//public functions

Renderer::Renderer(uint32_t window_width, uint32_t window_height){

  //perspective projection
  projection = glm::perspective(
     90.0f,      //fov
     1.0f,//(float)window_width/(float)window_height1,  //Aspect ratio
     0.1f,       //near clipping plane
     100.0f      //far clipping plane
     );

  m_window_width = window_width;
  m_window_height = window_height;
  
  //view matrix
  view = glm::lookAt(
     glm::vec3(0,0,-4), //camera position
     glm::vec3(0,0,0),  //look at position
     glm::vec3(0,-1,0)  //up vector
     );
  
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

void Renderer::render_model(const Model &model){
  model.render();
}

void Renderer::render_end(){
  SDL_GL_SwapWindow(main_window);
}

glm::mat4 Renderer::get_matrix(){
  return projection * view;
}

void Renderer::set_matrix(const glm::mat4 &model){
  glm::mat4 mvp = projection * view * model;
  shader.set_matrix( mvp );  
}

void Renderer::set_shader(Model &model){
  model.set_shader(shader);
}

void Renderer::cleanup(){
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
}

bool Renderer::set_gl_attributes(){

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  return true;
}
