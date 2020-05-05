#ifndef _CHUNK_CONTAINER_H_
#define _CHUNK_CONTAINER_H_

#include "chunk.h"
#include "voxel.h"

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <time.h>
#include <math.h>

#define CHUNK_POSITION_OFFSET 16000

#define VIEW_DISTANCE 8
#define TIME_PER_UPDATE 200.0f

class ChunkContainer{
  
public:

  ChunkContainer(int x_size,
		 int y_size,
		 int z_size);
  ~ChunkContainer();

  Voxel* get_voxel_at(glm::vec3 coords);
  Chunk* get_chunk_at(int x, int y, int z);
  
  int get_num_chunks();

  glm::vec3 get_position();

  void set_position(glm::vec3 position);

  bool in_view_distance(Chunk * chunk);
  
  Chunk *get_chunks();

private:
  
  int m_x_size, m_y_size, m_z_size;
  glm::vec3 m_position;

  float total_time;
 
  Chunk *chunks;
  
  void setup_chunk_data();
  void setup_chunk(int x_world,
		   int y_world,
		   int z_world);
  
  int get_chunk_index(int x, int y, int z);

};

#endif
