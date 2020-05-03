#ifndef _CHUNK_DATA_SRC_H_
#define _CHUNK_DATA_SRC_H_

#include "voxel.h"

class ChunkDataSrc{
 public:
  //returns a chunk from the chunk source
  virtual Voxel * get_chunk_data_at(int x, int y, int z) = 0;
  //returns 0 on succes
  virtual int trylock_chunk(int x, int y, int z) = 0;
  virtual void unlock_chunk(int x, int y, int z) = 0;
  
};

#endif
