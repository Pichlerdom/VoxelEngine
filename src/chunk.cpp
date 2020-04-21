#include "chunk.h"


void Chunk::init(){
  m_should_unload = false;
  m_is_loaded = false;
  m_is_setup = false;
  m_empty_chunk = false;
  m_mesh_updated = false;
  srand(time(NULL));
  mesh = new Mesh();
}

void Chunk::cleanup(){
  free(m_voxels);
  delete mesh;
}

void Chunk::update(){
  
}

void Chunk::unload(){
  free(m_voxels);

  mesh->reset();
  
  m_is_loaded = false;
  m_is_setup = false;
  m_mesh_updated = false;
  m_empty_chunk = false;
  m_should_unload = false;
}


void Chunk::update_mesh(){
  mesh->reset();
  
  generate_mesh();

  m_mesh_updated = true;
}

bool Chunk::is_mesh_updated(){
  return m_mesh_updated;
}

void Chunk::render(glm::mat4 mvp){
  mesh->translate( position);
  mesh->set_matrix(mvp * mesh->get_model());
  mesh->render();
}

void Chunk::set_shader(Shader _shader){
  mesh->set_shader(_shader);
}

void Chunk::set_position(int x, int y, int z){
  position.x = (float) x;
  position.y = (float) y;
  position.z = (float) z;
}

void Chunk::set_unload(){
  m_should_unload = true;
}

bool Chunk::should_unload(){
  return m_should_unload;
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
  for(int x = 0; x < CHUNK_SIZE; x++){
    for(int y = 0; y < CHUNK_SIZE; y++){
      for(int z = 0; z < CHUNK_SIZE; z++){
	if(x == 0)
	  get_voxel(x, y, z)->set_active(true);
      }
    }
  }
  m_is_setup = true;
}

bool Chunk::should_render(){
  return (!m_empty_chunk) && is_setup() && is_loaded();
}

void Chunk::update_empty_chunk_flag(){
  if(mesh->get_num_vertices() == 0 &&
     mesh->get_num_triangles() == 0){
    m_empty_chunk = true;
    printf("empty_chunk\n");
  }
}

bool Chunk::in_frustum(glm::mat4 M){
  glm::vec4 Pclip = M * mesh->get_model() * glm::vec4(position, 1.);
  
  return (glm::abs(Pclip.x) < Pclip.w &&
	  glm::abs(Pclip.y) < Pclip.w &&
	  0 < Pclip.z &&
	  Pclip.z < Pclip.w);			   
}

Voxel* Chunk::get_voxel(int32_t x, int32_t y, int32_t z){
  if(x < 0) x = 0;
  else if(x > CHUNK_SIZE-1) x = CHUNK_SIZE-1;
  if(y < 0) y = 0;
  else if(y > CHUNK_SIZE-1) y = CHUNK_SIZE-1;
  if(z < 0) z = 0;
  else if(z > CHUNK_SIZE-1) z = CHUNK_SIZE-1;
  
  return m_voxels + CHUNK_SIZE * CHUNK_SIZE * z + CHUNK_SIZE * y + x;
}

void Chunk::generate_mesh(){
  for(int x = 0 ; x < CHUNK_SIZE; x++){
    for(int y = 0 ; y < CHUNK_SIZE; y++){
      for(int z = 0 ; z < CHUNK_SIZE; z++){
	if(!get_voxel(x, y, z)->is_active())
	  continue;
	
	bool neg_x = true;
	if(x < CHUNK_SIZE - 1){
	  neg_x = get_voxel(x-1, y, z)->is_active();
	}

	bool pos_x = true;
	if(x > 0){
	  pos_x = get_voxel(x+1, y, z)->is_active();
	}

	bool neg_y = true;
	if(y < CHUNK_SIZE - 1){
	  neg_y = get_voxel(x, y-1, z)->is_active();
	}

	bool pos_y = true;
	if(y > 0){
	  pos_y = get_voxel(x, y+1, z)->is_active();
	}
	
	bool neg_z = true;
	if(z < CHUNK_SIZE - 1){
	  neg_z = get_voxel(x, y, z-1)->is_active();
	}

	bool pos_z = true;
	if(z > 0){
	  pos_z = get_voxel(x, y, z+1)->is_active();
	}
	
	
	generate_cube(x,y,z,
		      pos_x, neg_x,
		      pos_y, neg_y,
		      pos_z, neg_z);	
      }
    }
  }
  update_empty_chunk_flag();  
  mesh->finish_mesh();    
}

void Chunk::generate_cube(int x, int y, int z,
			  bool pos_x, bool neg_x,
			  bool pos_y, bool neg_y,
			  bool pos_z, bool neg_z){
  for(int i = 0; i < 8; i++){
    mesh->add_color(1.0, 0.0, 0.0, 1.0);
  }

  unsigned int v1 = mesh->add_vertex(x, y, z);
  unsigned int v2 = mesh->add_vertex(x, y, z + 1.0);
  unsigned int v3 = mesh->add_vertex(x, y + 1.0, z);
  unsigned int v4 = mesh->add_vertex(x, y + 1.0, z + 1.0);
  unsigned int v5 = mesh->add_vertex(x + 1.0, y, z);
  unsigned int v6 = mesh->add_vertex(x + 1.0, y, z + 1.0);
  unsigned int v7 = mesh->add_vertex(x + 1.0, y + 1.0, z);
  unsigned int v8 = mesh->add_vertex(x + 1.0, y + 1.0, z + 1.0);

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

  if(pos_y){
    //Top
    mesh->add_triangle(v3, v7, v8);
    mesh->add_triangle(v3, v4, v8);
  }
  if(neg_y){
    //Bottom
    mesh->add_triangle(v1, v2, v6);
    mesh->add_triangle(v1, v5, v6);  
  }
}
