#include "chunk_manager.h"

#include <glm/gtx/string_cast.hpp>
#define _DEBUG_

ChunkManager::ChunkManager(Renderer *renderer){
  m_renderer = renderer;
  m_force_visibility_update = false;
  chunk_container = new ChunkContainer(VIEW_DISTANCE * 2, VIEW_DISTANCE * 2, VIEW_DISTANCE * 2);
}

void ChunkManager::update(float dt,
			  glm::vec3 camera_position){
  update_load_list();
  
  update_setup_list();
  
  update_rebuild_list();


  chunk_container->set_position(camera_position.x- VIEW_DISTANCE,
				camera_position.y - VIEW_DISTANCE,
				camera_position.z - VIEW_DISTANCE);
  update_unload_list();
  
  update_visibility_list();

  if(camera_position != m_camera_position){
    update_render_list();
  }
}

void ChunkManager::render(glm::mat4 mvp){
  for (unsigned int i = 0;
       i < chunk_render_list.size();
       i++){
    chunk_render_list[i]->render(mvp);
  }
  chunk_render_list.clear();
}


void ChunkManager::update_load_list(){
  int chunks_loaded = 0;

  for(int i = 0;
      i < (int) chunk_load_list.size() &&
      NUM_ASYNC_CHUNKS_PER_FRAME > chunks_loaded;
      i++){
    Chunk* curr_chunk= chunk_load_list[i];
    
    if(!curr_chunk->is_loaded()){
      if(chunks_loaded != NUM_ASYNC_CHUNKS_PER_FRAME){
	curr_chunk->load();

	chunks_loaded++;
        m_force_visibility_update = true;
      }
    }
  }

  chunk_load_list.clear();
}

void ChunkManager::update_setup_list(){
  for(int i = 0; i < (int) chunk_setup_list.size(); i++){
    Chunk* curr_chunk = chunk_setup_list[i];
    if(curr_chunk->is_loaded() &&
       !curr_chunk->is_setup()){
      curr_chunk->setup();
      
      if(curr_chunk->is_setup()){
	m_force_visibility_update = true;
      }      
    }
  }
  chunk_setup_list.clear();
}

void ChunkManager::update_rebuild_list(){
  int chunks_rebuild = 0;
  for(int i = 0;
      i < (int) chunk_rebuild_list.size() &&
	chunks_rebuild < NUM_ASYNC_CHUNKS_PER_FRAME; ){
    Chunk *curr_chunk = chunk_rebuild_list[i];
    if(curr_chunk->is_loaded() && curr_chunk->is_setup()){
      curr_chunk->update_mesh();
      chunks_rebuild++;	
    }
  }
  chunk_rebuild_list.clear();
  printf("%d chunks rebuild\n",chunks_rebuild);
}


void ChunkManager::update_visibility_list(){
  Chunk *curr_chunk;
  chunk_visibility_list.clear();
  
  for(int chunk_x = 0; chunk_x < VIEW_DISTANCE * 2; chunk_x++){
    for(int chunk_y = 0; chunk_y < VIEW_DISTANCE * 2; chunk_y++){
      for(int chunk_z = 0; chunk_z < VIEW_DISTANCE * 2; chunk_z++){
	curr_chunk = chunk_container->get_chunks() +
	  chunk_x * VIEW_DISTANCE * VIEW_DISTANCE +
	  chunk_y * VIEW_DISTANCE +
	  chunk_z;
	if(curr_chunk == NULL) continue;
    
	if(curr_chunk->should_unload()){
	  chunk_unload_list.push_back(curr_chunk);
        }
	else if(!curr_chunk->is_loaded()){
	  chunk_load_list.push_back(curr_chunk);
	}
	else if(!curr_chunk->is_setup()){
	  chunk_setup_list.push_back(curr_chunk);
	}
	else if(!curr_chunk->is_mesh_updated()){
	  chunk_rebuild_list.push_back(curr_chunk);
	}
	else if(curr_chunk->should_render()){
	  chunk_visibility_list.push_back(curr_chunk);
	}
      }
    }
  }

}


void ChunkManager::update_render_list(){
  chunk_render_list.clear();

  glm::mat4 mat = m_renderer->get_matrix();
  for(unsigned int i= 0; i < chunk_visibility_list.size(); i++){
    Chunk* curr_chunk = chunk_visibility_list[i];
    if(curr_chunk != NULL){
      if(curr_chunk->is_loaded() &&
	 curr_chunk->is_setup() &&
	 curr_chunk->is_mesh_updated()){
	if(curr_chunk->in_frustum(mat)){
	  chunk_render_list.push_back(curr_chunk);
	  
	}
      }
    }
  }
  #ifdef _DEBUG_
  printf("V: %ld, RB: %ld, R: %ld, L: %ld, S %ld, U:%ld\n",
	 chunk_visibility_list.size(),
	 chunk_rebuild_list.size(),
	 chunk_render_list.size(),
	 chunk_load_list.size(),
	 chunk_setup_list.size(),
	 chunk_unload_list.size());
  #endif
}

void ChunkManager::update_unload_list(){
  int chunks_unloaded = 0;
  for(unsigned int i = 0;
      i < chunk_unload_list.size() &&
	NUM_ASYNC_CHUNKS_PER_FRAME > chunks_unloaded;
      i++){
    Chunk * curr_chunk = chunk_unload_list[i];
    if(curr_chunk->should_unload()){
      curr_chunk->unload();
      chunks_unloaded++;
    }
  }
  chunk_unload_list.clear();
}
