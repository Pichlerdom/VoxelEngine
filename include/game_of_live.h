#ifndef _GAME_OF_LIVE_H_
#define _GAME_OF_LIVE_H_

#include "chunk_data_src.h"
#include "chunk_manager.h"
#include "voxel.h"
#include "chunk.h"
#include "timer.h"

#include <math.h>
#include <pthread.h>
#include <stdint.h>

#define N 32

class GameOfLive: public ChunkDataSrc{
public:
  GameOfLive(ChunkManager *chunk_mgr);
  
  void update(float dt, uint32_t rule);
  void cleanup();
  
  void reset();
  virtual int trylock_chunk(int x, int y, int z);
  virtual void unlock_chunk(int x, int y, int z);
  virtual Voxel* get_chunk_data_at(int x, int y, int z);

private:
  
  struct GOLChunk{
    Voxel voxels[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    pthread_mutex_t mtx;
  };
  struct GOLChunk * chunks;

  ChunkManager *m_chunk_mgr;
  int m_z_offset = 0;
  bool apply_rule(uint32_t rule,int x, int y, int z);
  Voxel* get_voxel_at(int x, int y, int z);
  struct GOLChunk *get_chunk_at(int x, int y, int z);
  int lock_chunk(int x, int y, int z);  
  void init_chunks();
  
};





#endif
