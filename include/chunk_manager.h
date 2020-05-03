#ifndef _CHUNK_MANAGER_H_
#define _CHUNK_MANAGER_H_

#include "chunk.h"
#include "chunk_container.h"
#include "chunk_data_src.h"
#include "renderer.h"


#define GL3_PROTOTYPE 1
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <deque>

#define NUM_ASYNC_CHUNKS_PER_FRAME 128
#define NUM_CHUNK_UPDATES_PER_FRAME 128

class ChunkManager{
public:
  ChunkManager(Renderer *renderer);
  void update(float dt,
	      glm::vec3 camera_position);

  void push_chunk_update(int x, int y, int z);
  void render();
  void bind_data_src(ChunkDataSrc * src);
private:
  
  struct ChunkUpdate{
    int x,y,z;
  };

  pthread_mutex_t update_mutex;
  std::deque<struct ChunkUpdate> chunk_update_que;
  
  std::vector<Chunk *> chunk_load_list;
  std::vector<Chunk *> chunk_setup_list;
  std::vector<Chunk *> chunk_rebuild_list;
  std::vector<Chunk *> chunk_visibility_list;
  std::vector<Chunk *> chunk_render_list;
  std::vector<Chunk *> chunk_unload_list;

  
  ChunkContainer *chunk_container;
  ChunkDataSrc *m_src;
  
  glm::vec3 m_camera_view;
  glm::vec3 m_camera_position;
  
  Renderer *m_renderer;
  bool m_force_visibility_update;
  
  void handle_chunk_updates();
  void update_load_list();
  void update_setup_list();
  void update_rebuild_list();
  void update_visibility_list();
  void update_render_list();
  void update_unload_list();
};

#endif
