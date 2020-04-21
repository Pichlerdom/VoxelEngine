#ifndef _CHUNK_H_
#define _CHUNK_H_
#include "voxel.h"
#include "mesh.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


#define CHUNK_SIZE 16
#define CHUNK_DATA_SIZE CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

class Chunk{
public:
  void init();
  void cleanup();
  
  void update();
  void render(glm::mat4 mvp);

  void update_mesh();
  bool is_mesh_updated();
  
  bool is_loaded();
  void load();
  void unload();

  
  bool in_frustum(glm::mat4 M);
  
  bool should_render();

  bool is_setup();
  void setup();  

  void set_unload();
  bool should_unload();
  
  void update_empty_chunk_flag();
  Voxel* get_voxel(int32_t x, int32_t y, int32_t z);

  void set_shader(Shader _shader);

  void set_position(int x, int y, int z);
  glm::vec3 position;

private:
  void generate_mesh();
  void generate_cube(int x, int y, int z,
		     bool pos_x, bool neg_x,
		     bool pos_y, bool neg_y,
		     bool pos_z, bool neg_z);
  
  bool m_is_loaded;
  bool m_is_setup;
  bool m_mesh_updated;
  bool m_empty_chunk;
  bool m_should_unload;
  
  Mesh *mesh;

  Voxel* m_voxels;
  
};

#endif
