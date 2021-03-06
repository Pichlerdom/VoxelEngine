#include "chunk_manager.h"
//#define _DEBUG_

ChunkManager::ChunkManager(Renderer *renderer){
  m_renderer = renderer;
  m_force_visibility_update = false;
  update_mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_init(&update_mutex,NULL);
  
  chunk_container = new ChunkContainer(VIEW_DISTANCE * 2, VIEW_DISTANCE * 2, VIEW_DISTANCE * 2);
}
void ChunkManager::bind_data_src(ChunkDataSrc *src){
  m_src = src;
}

void ChunkManager::update(float dt,
			  glm::vec3 camera_position,
			  glm::vec3 mouse_pos){  
  update_load_list();
  
  update_setup_list();
  
  update_rebuild_list();

  chunk_container->set_position(camera_position);

  update_unload_list();
  
  update_visibility_list();

  if(m_force_visibility_update ||
     mouse_pos != m_mouse_pos ||
     camera_position != m_camera_position){

    update_render_list();

    m_force_visibility_update = false;

    m_camera_position = camera_position;
    m_mouse_pos = mouse_pos;
  }
  handle_chunk_updates();  
}

void ChunkManager::render(){
  m_renderer->push_matrix(glm::translate(-(glm::vec3(VIEW_DISTANCE,
						     VIEW_DISTANCE,
						     VIEW_DISTANCE))));

  glm::mat4 chunk_model;
  for (unsigned int i = 0;
       i < chunk_render_list.size();
       i++){

    chunk_model = chunk_render_list[i]->get_transform_mat(VIEW_DISTANCE, m_camera_position);
    m_renderer->push_matrix(chunk_model);
    
    chunk_render_list[i]->render(m_renderer);

    m_renderer->pop_matrix();
    
  }
  m_renderer->pop_matrix();
}

void ChunkManager::push_chunk_update(int x, int y, int z){  
    
  if(abs(x - m_camera_position.x) < (VIEW_DISTANCE * 2) &&
     abs(y - m_camera_position.y) < (VIEW_DISTANCE * 2) &&
     abs(z - m_camera_position.z) < (VIEW_DISTANCE * 2)){

    pthread_mutex_lock(&update_mutex);
    
    ChunkUpdate chunk_update;
  
    chunk_update.x = x;
    chunk_update.y = y;
    chunk_update.z = z;  

    chunk_update_que.push_front(chunk_update);
    pthread_mutex_unlock(&update_mutex);
  }    
}

void ChunkManager::handle_chunk_updates(){
  Chunk * chunk;
  pthread_mutex_lock(&update_mutex);
  #ifdef _DEBUG_
  printf("UD: %ld,",
	 chunk_update_que.size());
  #endif
  for(int i = 0;
      !chunk_update_que.empty(); //&& i < NUM_CHUNK_UPDATES_PER_FRAME;
      i++){
    ChunkUpdate update = chunk_update_que.back();
    chunk = chunk_container->get_chunk_at((int)update.x, (int)update.y, (int)update.z);
      
    if(chunk->is_loaded() && chunk->is_setup()){
      chunk->update();
    }
    
    chunk_update_que.pop_back();
  }
  pthread_mutex_unlock(&update_mutex);
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
      curr_chunk->setup_from_src(m_src);
      
      if(curr_chunk->is_setup()){
	m_force_visibility_update = true;
      }      
    }
  }
  chunk_setup_list.clear();
}

void ChunkManager::update_rebuild_list(){

  int chunks_rebuild = 0;
  glm::mat4 chunk_model;
  Chunk *curr_chunk;    
  m_renderer->push_matrix(glm::translate(-(glm::vec3(VIEW_DISTANCE,
						     VIEW_DISTANCE,
						     VIEW_DISTANCE))));

  
  for(int i = 0;
      i < (int) chunk_rebuild_list.size() &&
	chunks_rebuild < NUM_ASYNC_CHUNKS_PER_FRAME; i++){
    curr_chunk = chunk_rebuild_list[i];
    
    if(curr_chunk->is_loaded() && curr_chunk->is_setup()){
      chunk_model = curr_chunk->get_transform_mat(VIEW_DISTANCE,m_camera_position);
      m_renderer->push_matrix(chunk_model);
      
      if(m_renderer->in_frustum()){	  
	curr_chunk->update_mesh();
	chunks_rebuild++;
      }
      m_renderer->pop_matrix();
    }
  }
  chunk_rebuild_list.clear();
  m_renderer->pop_matrix();
}


void ChunkManager::update_visibility_list(){
  Chunk *curr_chunk;
  chunk_visibility_list.clear();

  for(int i = 0; i < chunk_container->get_num_chunks(); i++){  
    curr_chunk = chunk_container->get_chunks() + i;

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

    if(!curr_chunk->is_mesh_updated()){
      chunk_rebuild_list.push_back(curr_chunk);
    }
    if (curr_chunk->should_render()){
      chunk_visibility_list.push_back(curr_chunk);
    }
  }
#ifdef _DEBUG_
  printf("V: %ld, RB: %ld, R: %ld, L: %ld, S %ld, UL:%ld\n",
	 chunk_visibility_list.size(),
	 chunk_rebuild_list.size(),
	 chunk_render_list.size(),
	 chunk_load_list.size(),
	 chunk_setup_list.size(),
	 chunk_unload_list.size());
  #endif
}


void ChunkManager::update_render_list(){
  chunk_render_list.clear();
  m_renderer->push_matrix(glm::translate(-(glm::vec3(VIEW_DISTANCE,
						     VIEW_DISTANCE,
						     VIEW_DISTANCE))));
  glm::mat4 chunk_model;
  
  for(unsigned int i= 0; i < chunk_visibility_list.size(); i++){
    Chunk* curr_chunk = chunk_visibility_list[i];
    if(curr_chunk != NULL){
      if(curr_chunk->is_loaded() &&
	 curr_chunk->is_setup()){

	chunk_model = curr_chunk->get_transform_mat(VIEW_DISTANCE,m_camera_position);
	m_renderer->push_matrix(chunk_model);
	
	if(m_renderer->in_frustum()){
	  chunk_render_list.push_back(curr_chunk);
	}
	
	m_renderer->pop_matrix();
      }
    }
  }
  m_renderer->pop_matrix();  
}

void ChunkManager::update_unload_list(){
  int chunks_unloaded = 0;
  for(unsigned int i = 0;
      i < chunk_unload_list.size();
      i++){
    Chunk * curr_chunk = chunk_unload_list[i];
    if(curr_chunk->should_unload()){
      curr_chunk->unload();
      chunks_unloaded++;
    }
  }
  chunk_unload_list.clear();
}
