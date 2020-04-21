#include "chunk_container.h"


ChunkContainer::ChunkContainer(int x_size, int y_size, int z_size){
  m_x_size = x_size;
  m_y_size = y_size;
  m_z_size = z_size;
  m_x_pos = 0;
  m_y_pos = 0;
  m_z_pos = 0;  
  setup_chunk_data(x_size, y_size, z_size);

}

ChunkContainer::~ChunkContainer(){
  for(int i = 0; i < get_num_chunks(); i++){
    chunks[i].cleanup();
  }
  free(chunks);
}

void ChunkContainer::set_position(int x_pos, int y_pos, int z_pos){
  if(x_pos < 0) x_pos = 0;
  if(y_pos < 0) y_pos = 0;
  if(z_pos < 0) z_pos = 0;
  if(m_x_pos < x_pos){ //positive x
    if(x_pos - m_x_pos < 128){
      for(int x = m_x_pos; x < x_pos; x++){
	for(int y = 0; y < m_y_size; y++){
	  for (int z = 0; z < m_z_size; z++){
	    chunks[get_chunk_index(x,y,z)].set_unload();
	    chunks[get_chunk_index(x,y,z)].set_position(x + m_x_size,
							y+y_pos,
							z+z_pos);  
	  }
	}
      }
    }
    m_x_pos = x_pos;
  }  
  else if(m_x_pos > x_pos){//negative x
    if(m_x_pos - x_pos < 128){

      for(int x = x_pos; x < m_x_pos; x++){
	for(int y = 0; y < m_y_size; y++){
	  for (int z = 0; z < m_z_size; z++){
	    chunks[get_chunk_index(x,y,z)].set_unload();
	    chunks[get_chunk_index(x,y,z)].set_position(x,
							y+y_pos,
							z+z_pos);
	  }
	}
      }
    }
    m_x_pos = x_pos;
  }

  if(m_y_pos < y_pos){ //positive y
    if(y_pos - m_y_pos < 128){
      for(int x = 0; x < m_x_size; x++){
	for(int y = m_y_pos; y < y_pos; y++){
	  for (int z = 0; z < m_z_size; z++){
	    chunks[get_chunk_index(x,y,z)].set_unload();
	    chunks[get_chunk_index(x,y,z)].set_position(x+x_pos,
							y + m_y_size,
							z+z_pos);  
	  }
	}
      }
    }
    m_y_pos = y_pos;
  }
  else if(m_y_pos > y_pos){//negative y
    if(m_y_pos - y_pos  < 128){

      for(int x = 0; x < m_x_size; x++){
	for(int y = y_pos; y < m_y_pos; y++){
	  for (int z = 0; z < m_z_size; z++){
	    chunks[get_chunk_index(x,y,z)].set_unload();
	    chunks[get_chunk_index(x,y,z)].set_position(x+x_pos,
							y,
							z+z_pos);  
	  }
	}
      }
    }
    m_y_pos = y_pos;
  }

  if(m_z_pos < z_pos){ //positive z
    if( y_pos - m_y_pos  < 128){
      for(int x = 0; x < m_x_size; x++){
	for(int y = 0; y < m_y_size; y++){
	  for (int z = m_z_pos; z < z_pos; z++){
	    chunks[get_chunk_index(x,y,z)].set_unload();
	    chunks[get_chunk_index(x,y,z)].set_position(x+x_pos,
							y+y_pos,
							z+m_z_size);  
	  }
	}
      }
    }
    m_z_pos = z_pos;
  }
  else if(m_z_pos > z_pos){//negative z
    if(m_z_pos - z_pos  < 128){
      for(int x = 0; x < m_y_size; x++){
	for(int y = 0; y < m_y_size; y++){
	  for (int z = z_pos; z < m_z_pos; z++){
	    chunks[get_chunk_index(x,y,z)].set_unload();
	    chunks[get_chunk_index(x,y,z)].set_position(x+x_pos,
							y+y_pos,
							z);
	  }
	}
      }
    }
    m_z_pos = z_pos;
  }
}

Chunk* ChunkContainer::get_chunk_at(int x, int y, int z){
  if(x < m_x_pos || x > m_x_pos + m_x_size ||
     y < m_y_pos || y > m_y_pos + m_y_size ||
     z < m_z_pos || z > m_z_pos + m_z_size)
    return NULL;
  return chunks + get_chunk_index(x, y, z);
}

int ChunkContainer::get_num_chunks(){
  return m_x_size * m_y_size * m_z_size;
}
Chunk * ChunkContainer::get_chunks(){
  return chunks;
}

int ChunkContainer::get_chunk_index(int x, int y, int z){
  return ((x%m_x_size) * m_y_size * m_z_size +
	  (y%m_y_size) * m_z_size +
	  (z%m_z_size));
}

void ChunkContainer::setup_chunk_data(int x_size, int y_size, int z_size){
  chunks = (Chunk *) calloc(x_size * y_size * z_size,
			    sizeof(Chunk));
  for(int x = 0; x < x_size; x++){
    for(int y = 0; y < y_size; y++){
      for(int z = 0; z < z_size; z++){
	chunks[get_chunk_index(x,y,z)].init();
	chunks[get_chunk_index(x,y,z)].set_position(x,y,z);	
      }
    }
  }
}
