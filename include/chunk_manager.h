#ifndef _CHUNK_MANAGER_H_
#define _CHUNK_MANAGER_H_

#include "chunk.h"
#include "chunk_container.h"
#include "renderer.h"
#include <stdlib.h>
#include <stdio.h>

#include <vector>

#define NUM_ASYNC_CHUNKS_PER_FRAME 64

#define VIEW_DISTANCE 4

class ChunkManager{
public:
  ChunkManager(Renderer *renderer);
  void update(float dt,
	      glm::vec3 camera_position);

  
  void render(glm::mat4 mvp);
  
private:
  std::vector<Chunk *> chunk_load_list;
  std::vector<Chunk *> chunk_setup_list;
  std::vector<Chunk *> chunk_rebuild_list;
  std::vector<Chunk *> chunk_visibility_list;
  std::vector<Chunk *> chunk_render_list;
  std::vector<Chunk *> chunk_unload_list;
  
  ChunkContainer *chunk_container;

  glm::vec3 m_camera_view;
  glm::vec3 m_camera_position;
  
  Renderer *m_renderer;
  bool m_force_visibility_update;
  

  void update_load_list();
  void update_setup_list();
  void update_rebuild_list();
  void update_visibility_list();
  void update_render_list();
  void update_unload_list();
};

#endif
