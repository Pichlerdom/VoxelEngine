
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <pthread.h>

#include "math_utils.h"
#include "renderer.h"
#include "event_handler.h"
#include "chunk_manager.h"
#include "timer.h"
#include "game_of_live.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define MOVEMENT_SPEED 2.0f
#define MOUSE_SENSITIVITY 0.1f

#define INITIAL_POSITION glm::vec3(VIEW_DISTANCE,VIEW_DISTANCE,VIEW_DISTANCE-1.0f)

#define _DEBUG_

EventHandler handler;
float mouse_pos_x = 0;
float mouse_pos_y = 0;

Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT);

ChunkManager chunk_manager(&renderer);

GameOfLive * game_of_live = new GameOfLive(&chunk_manager);

glm::vec3 position = INITIAL_POSITION;
glm::vec3 light_pos(100.0f, 100.0f, 50.0f);

Timer *timer_voxel = new Timer();
Timer *timer_game = new Timer();

double tick_rate = 1000.0/1.0;

uint32_t rule = 0x00000000;
uint64_t count = 0;



void print_commands();
void update_model();
void handle_mouse_inputs(float dt);
void handle_keyboard_inputs(float dt);


void render(){
  
  renderer.render_start();

  chunk_manager.render();
  
  renderer.render_end();
  
}

void update(){

  while(!handler.WasQuit()){
    handler.Update();
    
    update_model();

    render();
  }
}
void update_model(){
  
  double delta_time = timer_voxel->tick();

#ifdef _DEBUG_
  printf("%.4lfms\n%.2f,%.2f,%.2f, rule:%d, ms_per_tick: %.2f\n", delta_time, position.x, position.y, position.z,rule,tick_rate);
#endif

  handle_keyboard_inputs(delta_time);
  handle_mouse_inputs(delta_time);

  chunk_manager.update(delta_time, position, glm::vec3(mouse_pos_x, mouse_pos_y, 1.0f));
}

void handle_mouse_inputs(float dt){
  SDL_Point new_mouse_pos = handler.GetMousePosition();
  mouse_pos_x = - new_mouse_pos.x * MOUSE_SENSITIVITY;
  mouse_pos_y = new_mouse_pos.y * MOUSE_SENSITIVITY;

  if(mouse_pos_y >= 90.0){
    mouse_pos_y = 89.9;
  } else if(mouse_pos_y <= -90.0){
    mouse_pos_y = -89.9;
  }
  
  renderer.update_view_matrix(mouse_pos_x, mouse_pos_y);  
}

void handle_keyboard_inputs(float dt){
    
  if(handler.IsKeyDown(SDLK_r)){
    position = INITIAL_POSITION;
    game_of_live->reset();
  }else{  
    float delta_forward = 0;
    float delta_right = 0;
    
    if(handler.IsKeyDown(SDLK_w)){
      delta_forward = ((float)dt)/1000;
    }else if(handler.IsKeyDown(SDLK_s)){
      delta_forward = ((float)-dt)/1000;
    }

    if(handler.IsKeyDown(SDLK_d)){
      delta_right = ((float)dt)/1000;
    }else if(handler.IsKeyDown(SDLK_a)){
      delta_right = ((float)-dt)/1000;
    }    
    position += renderer.get_front() * glm::vec3(delta_forward * MOVEMENT_SPEED);
    position += renderer.get_right() * glm::vec3(delta_right * MOVEMENT_SPEED);
  }
  
  count ++;

  if(count % 2 == 0){
    if(handler.IsKeyDown(SDLK_t)){
      rule+=2;
    }else if(handler.IsKeyDown(SDLK_g)){
      rule-=2;
    }  
    if(handler.IsKeyDown(SDLK_z)){
      rule+=16;
    }else if(handler.IsKeyDown(SDLK_h)){
      rule-=16;
    }
  }

  if(handler.IsKeyDown(SDLK_c)){
    tick_rate -= 10.0;
  }else if(handler.IsKeyDown(SDLK_v)){
    tick_rate += 10.0;
  }

  if(handler.IsKeyDown(SDLK_y)){
    tick_rate --;
  }else if(handler.IsKeyDown(SDLK_x)){
    tick_rate ++;
  }

  if(tick_rate < 0)
    tick_rate = 1;
}


void * game_update_loop(void * args){
  double dt = 0;
  //  SDL_Delay(2000.0);
  timer_game->start();  
  
  while(!handler.WasQuit()){
    game_of_live->update(dt,rule);
    dt = timer_game->tick();
    if(dt < tick_rate)
      SDL_Delay(tick_rate - dt);
  }

  game_of_live->cleanup();
  return NULL;
}

int main(int argc, char* argv[]){
  mouse_pos_x = 0;
  mouse_pos_y = 0;
  
  if(argc == 2){
    rule = std::stoi(argv[1]); 
  }
  //setup game thread
  pthread_t game_thread;
  pthread_create(&game_thread, NULL, game_update_loop, NULL);
    
  //setup voxel engine
  if(!renderer.init())
    return -1;

  if(!renderer.set_up_shader("flat.vert", "flat.frag"))
    printf("Shader setup failed somehow!!");

  renderer.set_light(light_pos);

  timer_voxel->start();
  //start updating voxel engine 
  update();


  pthread_join(game_thread,NULL);
  renderer.cleanup();

  delete timer_voxel;

  return 0;  
}


