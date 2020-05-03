#include "game_of_live.h"


GameOfLive::GameOfLive(ChunkManager *chunk_mgr){  
  m_chunk_mgr = chunk_mgr;
  m_chunk_mgr->bind_data_src(this);
  srand(time(NULL));
  init_chunks();
}

void GameOfLive::init_chunks(){
  chunks = (struct GOLChunk*) calloc(N*N*N, sizeof(struct GOLChunk));

  
  struct GOLChunk *curr_chunk;

  Voxel *curr_voxel;
  for(int chunk_i = 0; chunk_i < N*N*N; chunk_i++){
    curr_chunk = chunks + chunk_i;
    
    curr_chunk->mtx = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_init(&curr_chunk->mtx,NULL);
    
    for(int voxel_i = 0; voxel_i < CHUNK_DATA_SIZE; voxel_i++){
      curr_voxel = &curr_chunk->voxels[0][0][0] + voxel_i;
      curr_voxel->set_active(false);
    }
  }
  
  get_voxel_at(0,0,0)->set_active(true);
  
}
void GameOfLive::cleanup(){

  for(int i = 0; i < N*N*N; i++){
    pthread_mutex_destroy(&chunks[i].mtx);
  }  
  free(chunks);

}


Voxel* GameOfLive::get_voxel_at(int x, int y, int z){
  int x_pos, y_pos, z_pos;
  
  if(x < 0){
    x_pos = (N * CHUNK_SIZE) - ((int)abs(x) % (N * CHUNK_SIZE));
  }else{
    x_pos = x%(N * CHUNK_SIZE);
  }

  if(y < 0){
    y_pos = (N * CHUNK_SIZE) - ((int)abs(y) % (N * CHUNK_SIZE));
  }else{
    y_pos = y%(N * CHUNK_SIZE);
  }
  
  if(z < 0){
    z_pos = (N * CHUNK_SIZE) - ((int)abs(z) % (N * CHUNK_SIZE));
  }else{
    z_pos = z%(N * CHUNK_SIZE);
  }

  struct GOLChunk * chunk = get_chunk_at(floor(x_pos/CHUNK_SIZE),
					 floor(y_pos/CHUNK_SIZE),
					 floor(z_pos/CHUNK_SIZE));
  return &chunk->voxels[z_pos%CHUNK_SIZE][y_pos%CHUNK_SIZE][x_pos%CHUNK_SIZE];
  
  
}

struct GameOfLive::GOLChunk * GameOfLive::get_chunk_at(int x, int y, int z) {
  int x_pos, y_pos, z_pos;  
  if(x < 0){
    x_pos = (N) - ((int)abs(x) % (N));
  }else{
    x_pos = x%(N);
  }

  if(y < 0){
    y_pos = (N) - ((int)abs(y) % (N));
  }else{
    y_pos = y%(N);
  }
  
  if(z < 0){
    z_pos = (N) - ((int)abs(z) % (N));
  }else{
    z_pos = z%(N);
  }

  return chunks + x_pos * N * N + y_pos * N + z_pos;
}

int GameOfLive::trylock_chunk(int x, int y, int z){
  struct GOLChunk * chunk = get_chunk_at(x,y,z);
  int res = pthread_mutex_trylock(&chunk->mtx);
  return res;
}

int GameOfLive::lock_chunk(int x, int y, int z){
  struct GOLChunk * chunk = get_chunk_at(x,y,z);
  int res = pthread_mutex_lock(&chunk->mtx);
  return res;
}

void GameOfLive::unlock_chunk(int x, int y, int z){
  struct GOLChunk * chunk = get_chunk_at(x,y,z);
  pthread_mutex_unlock(&chunk->mtx);
}


Voxel * GameOfLive::get_chunk_data_at(int x, int y, int z){ 
  return &get_chunk_at(x, y, z)->voxels[0][0][0];
}

bool GameOfLive::apply_rule(uint32_t rule, int x, int y, int z){
  uint8_t hood = 0;
  hood |= (get_voxel_at(x,y,z)->is_active()<<0);
  hood |= (get_voxel_at(x,y+1,z)->is_active()<<1);
  hood |= (get_voxel_at(x,y-1,z)->is_active()<<2);
  hood |= (get_voxel_at(x+1,y,z)->is_active()<<3);
  hood |= (get_voxel_at(x-1,y,z)->is_active()<<4);

  bool res = (rule & (1<<hood))!=0;
  Voxel *v = get_voxel_at(x,y,z+1);
  if(res != v->is_active()){
    get_voxel_at(x,y,z+1)->set_active(res);
    return true;
  }
  return false;
}

void GameOfLive::reset(){
  struct GOLChunk *curr_chunk;
  Voxel *curr_voxel;
  bool did_change = false;
  for(int x_chunk = 0; x_chunk < N; x_chunk++){
    for(int y_chunk = 0; y_chunk < N; y_chunk++){
      for(int z_chunk = 0; z_chunk < N; z_chunk++){
	curr_chunk = get_chunk_at(x_chunk,y_chunk,z_chunk);    

        
	did_change = false;
	for(int voxel_i = 0; voxel_i < CHUNK_DATA_SIZE; voxel_i++){
	  curr_voxel = &curr_chunk->voxels[0][0][0] + voxel_i;
	  if(curr_voxel->is_active()){
	    curr_voxel->set_active(false);
	    did_change = true;
	  }
	}
	if(did_change)
	  m_chunk_mgr->push_chunk_update(x_chunk,
					 y_chunk,
					 z_chunk);
	
      }
    }
  }
  get_voxel_at(0,0,0)->set_active(true);

  m_chunk_mgr->push_chunk_update(0,0,0);

  m_z_offset = 0;
}

void GameOfLive::update(float dt, uint32_t rule){
  bool did_change = false;
  for(int x_chunk = 0; x_chunk < N; x_chunk++){
    for(int y_chunk = 0; y_chunk < N; y_chunk++){
      int z_chunk = (m_z_offset - m_z_offset%CHUNK_SIZE)/CHUNK_SIZE;

      lock_chunk(x_chunk, y_chunk, z_chunk);

      did_change = false;
      for(int x = 0; x < CHUNK_SIZE; x++){
	for(int y = 0; y < CHUNK_SIZE; y++){
	  if(apply_rule(rule,
			x_chunk * CHUNK_SIZE + x,
			y_chunk * CHUNK_SIZE + y,
			m_z_offset)){
	    did_change = true;
	  }

	}
      }

      unlock_chunk(x_chunk, y_chunk, z_chunk);
      
      if(did_change)
	m_chunk_mgr->push_chunk_update(x_chunk, y_chunk, z_chunk);
    }
  }
  m_z_offset++;
}

