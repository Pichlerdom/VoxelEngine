#include "chunk_container.h"

ChunkContainer::ChunkContainer(int x_size,
			       int y_size,
			       int z_size){
  m_x_size = x_size;
  m_y_size = y_size;
  m_z_size = z_size;
  m_position = glm::vec3(0);
  
  setup_chunk_data();

  srand(122354);    
}

ChunkContainer::~ChunkContainer(){
  for(int i = 0; i < get_num_chunks(); i++){
    chunks[i].cleanup();
  }
  free(chunks);
}

void ChunkContainer::set_position(glm::vec3 position){
  glm::vec3 position_floor = glm::floor(position);
  int x_pos = (int) position_floor.x;
  int y_pos = (int) position_floor.y;
  int z_pos = (int) position_floor.z;
  
  int m_x_pos = (int) m_position.x;
  int m_y_pos = (int) m_position.y;
  int m_z_pos = (int) m_position.z;
  
  if(m_x_pos < x_pos){ //positive x    
    for(int x = m_x_pos; x < x_pos; x++){
      for(int y = y_pos; y < y_pos + m_y_size; y++){
	for(int z = z_pos; z < z_pos + m_z_size; z++){
	  setup_chunk(x, y, z);
	  
	}
      }
    }
  }  
  else if(m_x_pos > x_pos){//negative x
    for(int x = x_pos; x < m_x_pos; x++){
      for(int y = y_pos; y < y_pos + m_y_size; y++){
	for(int z = z_pos; z < z_pos + m_z_size; z++){
	  setup_chunk(x + m_x_size, y, z);
	}
      }
    }
  }

  if(m_y_pos < y_pos){ //positive y
    for(int x = x_pos; x < x_pos + m_x_size; x++){
      for(int y = m_y_pos; y < y_pos; y++){
	for(int z = z_pos; z < z_pos + m_z_size; z++){
	  setup_chunk(x, y, z);
	}
      }
    }  
  }
  else if(m_y_pos > y_pos){//negative y
    for(int x = x_pos; x < x_pos + m_x_size; x++){
      for(int y = y_pos; y < m_y_pos; y++){
	for(int z = z_pos; z < z_pos + m_z_size; z++){
	  setup_chunk(x, y + m_y_size, z);
	  
	}
      }
    }
  }
  
  if(m_z_pos < z_pos){ //positive z
    for(int x = x_pos; x < x_pos + m_x_size; x++){
      for(int y = y_pos; y < y_pos + m_y_size; y++){
	for(int z = m_z_pos; z < z_pos; z++){
	  setup_chunk(x, y, z);
	}
      }
    }
  }
  else if(m_z_pos > z_pos){//negative z
    for(int x = x_pos; x < x_pos + m_x_size; x++){
      for(int y = y_pos; y < y_pos + m_y_size; y++){
	for(int z = z_pos; z < m_z_pos; z++){
	  setup_chunk(x, y, z + m_z_size);
	}
      }
    }
  }
  m_position = position_floor;
}


Chunk* ChunkContainer::get_chunk_at(int x,
				    int y,
				    int z){
  return chunks + get_chunk_index(x, y, z);
}

glm::vec3 ChunkContainer::get_position(){
  return m_position;
}

bool ChunkContainer::in_view_distance(Chunk* chunk){
  glm::vec3 div = chunk->get_world_position() - get_position();
  return (div.x > 0 && div.x < m_x_size &&
	  div.y > 0 && div.y < m_y_size &&
	  div.z > 0 && div.z < m_z_size);
}

int ChunkContainer::get_num_chunks(){
  return m_x_size * m_y_size * m_z_size;
}


Chunk * ChunkContainer::get_chunks(){
  return chunks;
}

int ChunkContainer::get_chunk_index(int x, int y, int z){
  int x_pos, y_pos, z_pos;
  if(x < 0){
    x_pos = m_x_size - ((int)abs(x) % m_x_size);
  }else{
    x_pos = x%m_x_size;
  }

  if(y < 0){
    y_pos = m_y_size - ((int)abs(y) % m_y_size);
  }else{
    y_pos = y%m_y_size;
  }
  
  if(z < 0){
    z_pos = m_z_size - ((int)abs(z) % m_z_size);
  }else{
    z_pos = z%m_z_size;
  }
    
  return ((x_pos) * m_y_size * m_z_size +
	  (y_pos) * m_z_size +
	  (z_pos));
}

void ChunkContainer::setup_chunk_data(){
  chunks = (Chunk *) calloc(m_x_size * m_y_size * m_z_size,
			    sizeof(Chunk));

  for(int x = 0; x < m_x_size; x++){
    for(int y = 0; y < m_y_size; y++){
      for(int z = 0; z < m_z_size; z++){
	Chunk *chunk = get_chunk_at(x, y, z);
	
	chunk->init();
	setup_chunk(x, y, z);
				    	
      }
    }
  }
}

void ChunkContainer::setup_chunk(int x_world,
				 int y_world,
				 int z_world){
  
  Chunk* chunk = get_chunk_at(x_world, y_world, z_world); 
  
  glm::vec3 chunk_pos = chunk->get_world_position();
  if(abs(chunk_pos.x - x_world) > 1 &&
     abs(chunk_pos.y - y_world) > 1 &&
     abs(chunk_pos.z - z_world) > 1){
    chunk->set_world_position(x_world, y_world, z_world);
    chunk->set_unload();
  }
}
