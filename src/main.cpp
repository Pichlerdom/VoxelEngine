#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "math_utils.h"
#include "renderer.h"
#include "event_handler.h"
#include "chunk_manager.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define FPS 60
#define MS_PER_FRAME 1000/FPS

EventHandler handler;

Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT);

ChunkManager chunk_manager(&renderer);
glm::vec3 position(1.0f);

void print_commands();
void update_model();

void render(){

  uint32_t start_time = SDL_GetTicks();
  renderer.render_start();
  
  glm::mat4 mvp = renderer.get_matrix();

  chunk_manager.render(mvp);
  
  /*glm::mat4 model = cube.get_model();
    
  for (int x = 0; x < 32; x++){
    for (int y = 0; y < 32; y++){
      for (int z = 0; z < 32; z++){
	glm::vec3 axis(0.0f);
	axis.x = x;
	axis.y = y;
	axis.z = z;
	renderer.set_matrix( model * glm::translate(glm::mat4(1.f),axis) );
	renderer.render_model(cube);
      }
    }
    }*/

  
  renderer.render_end();
  int delta_time = SDL_GetTicks() - start_time;
  printf("%d: %d\n ", delta_time, MS_PER_FRAME);
  
  if(delta_time < MS_PER_FRAME){
    SDL_Delay(MS_PER_FRAME - delta_time);
  }  
}

void update(){

  while(!handler.WasQuit()){
    handler.Update();
    
    update_model();

    render();
  }
}

void update_model(){
  glm::vec3 axis = Math::GetAxis(handler);
  
  if(handler.IsKeyDown(SDLK_r)){
    position.x = 0;
    position.y = 0;
    position.z = 0;
    chunk_manager.update(0, position);
  }else{  
    chunk_manager.update(0, position);
    printf("%f,%f,%f\n",position.x,position.y,position.z);

    position += axis;
  }
}



int main(void){
  if(!renderer.init())
    return -1;

  if(!renderer.set_up_shader("simple.vert", "simple.frag"))
    printf("Shader setup failed somehow!!");
  
  position.x = 2000;
  position.y = 2000;
  position.z = 2000;
  
  update();

  renderer.cleanup();

  return 0;
  
}


