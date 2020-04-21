#ifndef _CHUNK_CONTAINER_H_
#define _CHUNK_CONTAINER_H_

#include "chunk.h"

#include <stdlib.h>
#include <stdio.h>
#include <vector>


class ChunkContainer{
  
public:

  ChunkContainer(int x_size, int y_size, int z_size);
  ~ChunkContainer();
  
  Chunk* get_chunk_at(int x, int y, int z);
  int get_num_chunks();
  void set_position(int x, int y, int z);
  Chunk *get_chunks();

private:
  int m_x_size, m_y_size, m_z_size;
  int m_x_pos, m_y_pos, m_z_pos;
  Chunk *chunks;
  
  void setup_chunk_data(int x_size, int y_size, int z_size);
  int get_chunk_index(int x, int y, int z);

};

#endif
