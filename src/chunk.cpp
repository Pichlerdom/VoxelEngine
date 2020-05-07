#include "chunk.h"


void Chunk::init(){
  m_should_unload = false;
  m_is_loaded = false;
  m_is_setup = false;
  m_empty_chunk = false;
  m_mesh_updated = false;
  m_has_mesh = false;
  srand(time(NULL));
  mesh = new Mesh();
}

void Chunk::cleanup(){
  free(m_voxels);
  delete mesh;
}

void Chunk::update(){
  if(m_is_loaded && m_is_loaded)
    m_is_setup = false;
}


bool Chunk::is_mesh_updated(){
  return m_mesh_updated;
}

void Chunk::render(Renderer *renderer){
  renderer->render_mesh(mesh);
}


void Chunk::set_render_position(int x, int y, int z){
  position_render[0] = (float)x;
  position_render[1] = (float)y;
  position_render[2] = (float)z; 
}

void Chunk::set_world_position(int x, int y, int z){  
  position_world[0] = x;
  position_world[1] = y;
  position_world[2] = z; 
}

glm::mat4 Chunk::get_transform_mat(int view_distance, glm::vec3 camera_position){
  glm::vec3 chunk_offset;
  glm::vec3 floor_camera_position = glm::floor(camera_position);
  int x = position_world[0] - floor_camera_position.x;
  int y = position_world[1] - floor_camera_position.y;
  int z = position_world[2] - floor_camera_position.z;
  
  int x_pos, y_pos, z_pos;
  
  if(x < 0){
    x_pos = (view_distance * 2) - ((int)abs(x) % (view_distance * 2));
  }else{
    x_pos = x%(view_distance * 2);
  }

  if(y < 0){
    y_pos = (view_distance * 2) - ((int)abs(y) % (view_distance * 2));
  }else{
    y_pos = y%(view_distance * 2);
  }
  
  if(z < 0){
    z_pos = (view_distance * 2) - ((int)abs(z) % (view_distance * 2));
  }else{
    z_pos = z%(view_distance * 2);
  }

  chunk_offset = glm::vec3(x_pos, y_pos, z_pos);
  
  return glm::translate(chunk_offset) * glm::translate(glm::floor(camera_position) - camera_position);
}

glm::vec3 Chunk::get_world_position(){
  return glm::vec3((float)position_world[0],
		   (float)position_world[1],
		   (float)position_world[2]);
}

glm::vec3 Chunk::get_render_position(){
  return glm::vec3((float)position_render[0],
		   (float)position_render[1],
		   (float)position_render[2]);
}

void Chunk::set_unload(){
  if(m_is_loaded)
    m_should_unload = true;
}

bool Chunk::should_unload(){
  return m_should_unload;
}

void Chunk::unload(){
  free(m_voxels);
  
  mesh->reset();
  m_has_mesh = false;
  m_is_loaded = false;
  m_is_setup = false;
  m_mesh_updated = false;
  m_empty_chunk = false;
  m_should_unload = false;
}

bool Chunk::is_loaded(){
  return m_is_loaded;
}

void Chunk::load(){
  m_voxels = (Voxel *) calloc(CHUNK_DATA_SIZE, sizeof(Voxel));
  for(int i = 0; i < CHUNK_DATA_SIZE; i++){
    m_voxels[i].init();
  }  
  m_is_loaded = true;  
}

bool Chunk::is_setup(){
  return m_is_setup;
}

void Chunk::setup(){
  for(int i = 0; i < CHUNK_DATA_SIZE; i++){
    if(rand()%32 == 0)
      m_voxels[i].set_active(true);
  }
  m_is_setup = true;
  m_mesh_updated = false;
}

void Chunk::setup_from_src(ChunkDataSrc *src){
  int x = position_world[0];
  int y = position_world[1];
  int z = position_world[2];
  
  int res = src->trylock_chunk(x, y, z);
  if(res == 0){
    Voxel* src_voxels = src->get_chunk_data_at( x, y, z);
    memcpy(m_voxels, src_voxels, CHUNK_DATA_SIZE * sizeof(Voxel));

    src->unlock_chunk(x, y, z);
    m_is_setup = true;
    m_mesh_updated = false;
    
  }
}


bool Chunk::should_render(){
  return m_has_mesh && (!m_empty_chunk);
}

void Chunk::update_empty_chunk_flag(){
  if(mesh->get_num_vertices() == 0 &&
     mesh->get_num_triangles() == 0){
    m_empty_chunk = true;
  }else{
    m_empty_chunk = false;
  }
}

Voxel* Chunk::get_voxel(int x, int y, int z){ 
  return m_voxels + CHUNK_SIZE * CHUNK_SIZE * z + CHUNK_SIZE * y + x;
}

void Chunk::update_mesh(){
  mesh->reset();  
  generate_mesh();
  m_mesh_updated = true;
  m_has_mesh = true;
}

void Chunk::generate_mesh(){
  float voxel_size = 1.0/CHUNK_SIZE;  
  int *temp_prev;
  
  int *prev_x[2];
  prev_x[0] = (int *) calloc((CHUNK_SIZE + 1) * (CHUNK_SIZE + 1), sizeof(int));
  prev_x[1] = (int *) calloc((CHUNK_SIZE + 1) * (CHUNK_SIZE + 1), sizeof(int));
  for(int i = 0 ; i < 2; i++){
    for(int x; x < CHUNK_SIZE + 1; x++){
      for(int y; y < CHUNK_SIZE + 1; y++){
	prev_x[i][y] = -1;
      }
    }
  }
  
  int *prev_y[2];
  prev_y[0] = (int *) calloc(CHUNK_SIZE + 1, sizeof(int));
  prev_y[1] = (int *) calloc(CHUNK_SIZE + 1, sizeof(int));
  for(int i = 0 ; i < 2; i++){
    for(int y = 0; y < CHUNK_SIZE + 1; y++){
      prev_y[i][y] = -1;
    }
  }
  
  int prev_z = -1;

  int idx = -1;
  const int8_t * normal_idx;
  uint8_t adj = 0;
  const GLfloat * normal;
  
  for(int x = 0; x < CHUNK_SIZE; x++){  
    for(int y = 0; y < CHUNK_SIZE; y++){
      for(int z = 0; z < CHUNK_SIZE; z++){
	adj = get_adj_voxels(x,y,z);
	idx = -1;
	
	if(adj == 0 || adj == 255){                           //all active or all not active -> no vertex
	  prev_x[BACK][(y + 1) * (CHUNK_SIZE + 1) + z + 1] = -1;
	  prev_y[BACK][z + 1] = -1;
	  prev_z = -1;
	  continue;
	}
	
        idx = mesh->add_vertex(((float)x * voxel_size),  //add vertex
			       ((float)y * voxel_size),
			       ((float)z * voxel_size));
	mesh->add_color(1.0, 0.0, 0.0, 1.0);

	normal_idx = normal_idx_table + adj;
	normal = normal_lookup_table[*normal_idx];
	mesh->add_normal(normal[0], normal[1], normal[2]);
	
	if(((adj & NX_NY_NZ) != 0) !=
	   ((adj & PX_NY_NZ) != 0)){ //X PLANE
	  mesh->add_triangle(idx,
			     prev_y[FRONT][z],
			     prev_y[FRONT][z + 1]);
	  mesh->add_triangle(idx,
			     prev_y[FRONT][z],
			     prev_z);
	}
	
	if(((adj & NX_NY_NZ) != 0) != 
	   ((adj & NX_PY_NZ) != 0)){ //Y PLANE
	  mesh->add_triangle(idx,
			     prev_x[FRONT][(y + 1) * (CHUNK_SIZE + 1) + z],
			     prev_z);
	  mesh->add_triangle(idx,
			     prev_x[FRONT][(y + 1) * (CHUNK_SIZE + 1) + z],
			     prev_x[FRONT][(y + 1) * (CHUNK_SIZE + 1) + z + 1]);
	  
	}
	
	if(((adj & NX_NY_NZ) != 0) != 
	   ((adj & NX_NY_PZ) != 0)){ //Z PLANE
	  mesh->add_triangle(idx,
			     prev_x[FRONT][(y + 1) * (CHUNK_SIZE + 1) + z + 1],
			     prev_x[FRONT][y * (CHUNK_SIZE + 1) + z + 1]);
	  mesh->add_triangle(idx,
			     prev_y[FRONT][z + 1],
			     prev_x[FRONT][y * (CHUNK_SIZE + 1) + z + 1]);
	}
	   
	prev_x[BACK][(y + 1) * (CHUNK_SIZE + 1) + z + 1] = idx;
	prev_y[BACK][z + 1] = idx;
	prev_z = idx;
      }
      
      temp_prev = prev_y[FRONT];
      prev_y[FRONT] = prev_y[BACK];
      prev_y[BACK] = temp_prev;
    }
    
    temp_prev = prev_x[FRONT];
    prev_x[FRONT] = prev_x[BACK];
    prev_x[BACK] = temp_prev;
  }

  update_empty_chunk_flag();
  if(!m_empty_chunk)  
    mesh->finish_mesh();

  free(prev_x[BACK]);
  free(prev_x[FRONT]);
  free(prev_y[BACK]);
  free(prev_y[FRONT]);  
}

uint8_t Chunk::get_adj_voxels(int in_x, int in_y, int in_z){
  uint8_t adj = 0;
  int offset = 0;
  for(int x = in_x -1; x < in_x + 1; x++){
    for(int y = in_y - 1; y < in_y + 1; y++){
      for(int z = in_z - 1; z < in_z + 1; z++){
	adj |= (is_voxel_active(x,y,z)<<offset);
	offset++;
      }
    }
  }
  return adj;
}

bool Chunk::is_voxel_active(int x, int y, int z){
  if(x < 0 || x >= CHUNK_SIZE ||
     y < 0 || y >= CHUNK_SIZE ||
     z < 0 || z >= CHUNK_SIZE)
    return false;

  return get_voxel(x,y,z)->is_active();
}



/*
void Chunk::generate_mesh(){
  float voxel_size = 1.0/CHUNK_SIZE;
  for(int x = 0 ; x < CHUNK_SIZE; x++){
    for(int y = 0 ; y < CHUNK_SIZE; y++){
      for(int z = 0 ; z < CHUNK_SIZE; z++){
	if(!get_voxel(x, y, z)->is_active())
	  continue;
	
	bool neg_x = false;
	if(x > 0){
	  neg_x = get_voxel(x-1, y, z)->is_active();
	}

	bool pos_x = false;
	if(x < CHUNK_SIZE - 1){	
	  pos_x = get_voxel(x+1, y, z)->is_active();
	}

	bool neg_y = false;
	if(y > 0){
	  neg_y = get_voxel(x, y-1, z)->is_active();
	}

	bool pos_y = false;
	if(y < CHUNK_SIZE - 1){
	  pos_y = get_voxel(x, y+1, z)->is_active();
	}
	
	bool neg_z = false;
	if(z > 0){
	  neg_z = get_voxel(x, y, z-1)->is_active();
	}

	bool pos_z = false;
	if(z < CHUNK_SIZE - 1){
	  pos_z = get_voxel(x, y, z+1)->is_active();
	}
	
  	
	generate_cube(voxel_size * x,
		      voxel_size * y,
		      voxel_size * z,
		      pos_x, neg_x,
		      pos_y, neg_y,
		      pos_z, neg_z);	
      }
    }
  }

  update_empty_chunk_flag();
  if(!m_empty_chunk)
    mesh->finish_mesh();    
}

void Chunk::generate_cube(GLfloat x, GLfloat y, GLfloat z,
			  bool pos_x, bool neg_x,
			  bool pos_y, bool neg_y,
			  bool pos_z, bool neg_z){
  if(!pos_x && !pos_y && !pos_z &&
     !neg_x && !neg_y && !neg_z)
    return;
  for(int i = 0; i < 8; i++){
    mesh->add_color(1, 0, 0, 1.0);
  }

  float voxel_size = 1.00/CHUNK_SIZE;
  unsigned int v1 = mesh->add_vertex(x, y, z);
  mesh->add_normal(-1,-1,-1);
  unsigned int v2 = mesh->add_vertex(x, y, z + voxel_size);
  mesh->add_normal(-1,-1,1);
  unsigned int v3 = mesh->add_vertex(x, y + voxel_size, z);
  mesh->add_normal(-1,1,-1);
  unsigned int v4 = mesh->add_vertex(x, y + voxel_size, z + voxel_size);
  mesh->add_normal(-1,1,1);
  unsigned int v5 = mesh->add_vertex(x + voxel_size, y, z);
  mesh->add_normal(1,-1,-1);
  unsigned int v6 = mesh->add_vertex(x + voxel_size, y, z + voxel_size);
  mesh->add_normal(1,-1,1);
  unsigned int v7 = mesh->add_vertex(x + voxel_size, y + voxel_size, z);
  mesh->add_normal(1,1,-1);
  unsigned int v8 = mesh->add_vertex(x + voxel_size, y + voxel_size, z + voxel_size);
  mesh->add_normal(1,1,1);
    
  if(!neg_z){
    //Front
    mesh->add_triangle(v1, v5, v7); 
    mesh->add_triangle(v1, v3, v7);
  }
  if(!pos_z){
    //Back
    mesh->add_triangle(v2, v6, v8);
    mesh->add_triangle(v2, v4, v8);
  }

  if(!pos_x){
    //Right
    mesh->add_triangle(v5, v6, v8);
    mesh->add_triangle(v5, v7, v8);
  }
  if(!neg_x){
    //Left
    mesh->add_triangle(v1, v3, v4);
    mesh->add_triangle(v1, v2, v4);
  }

  if(!pos_y){
    //Top
    mesh->add_triangle(v3, v7, v8);
    mesh->add_triangle(v3, v4, v8);
  }
  if(!neg_y){
    //Bottom
    mesh->add_triangle(v1, v2, v6);
    mesh->add_triangle(v1, v5, v6);  
  }
}
*/
