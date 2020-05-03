#ifndef _CHUNK_H_
#define _CHUNK_H_

#include "renderer.h"
#include "voxel.h"
#include "mesh.h"
#include "chunk_data_src.h"

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
  void render(Renderer *renderer);

  void update_mesh();
  bool is_mesh_updated();
  
  bool is_loaded();
  void load();
  void unload();

  
  bool in_frustum(glm::mat4 M);
  
  bool should_render();

  bool is_setup();
  void setup();  
  void setup_from_src(ChunkDataSrc *src);
  void set_unload();
  bool should_unload();
  
  void update_empty_chunk_flag();
  Voxel* get_voxel(int x, int y, int z);

  void set_render_position(int x, int y, int z);
  void set_world_position(int x, int y, int z);

  glm::mat4 get_transform_mat(int view_distance, glm::vec3 camera_position);

  glm::vec3 get_world_position();
  glm::vec3 get_render_position();

private:
  void generate_mesh();
  void generate_cube(GLfloat x, GLfloat y, GLfloat z,
		     bool pos_x, bool neg_x,
		     bool pos_y, bool neg_y,
		     bool pos_z, bool neg_z);
  
  bool m_is_loaded;
  bool m_is_setup;
  bool m_mesh_updated;
  bool m_has_mesh;
  bool m_empty_chunk;
  bool m_should_unload;

  
  int position_world[3];
  float position_render[3];
  Mesh *mesh;

  Voxel* m_voxels;
  
};


#endif
