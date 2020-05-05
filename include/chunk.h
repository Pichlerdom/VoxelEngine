
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

#define FRONT 0
#define BACK 1



const GLfloat normal_lookup_table[27][3] =
  {
   {0,0,0},   //0
   {1,1,1},   //1 
   {1,1,-1},  //2
   {1,1,0},   //3

   {1,-1,1},  //4
   {1,0,1},   //5
   {1,0,0},   //6
   {1,-1,-1}, //7
   
   {1,0,-1},  //8
   {1,-1,0},  //9
   {-1,1,1},  //10
   {0,1,1},   //11

   {0,1,0},   //12
   {0,0,1},   //13
   {-1,-1,1}, //14
   {-1,1,-1}, //15

   {0,0,-1},  //16
   {0,-1,-1}, //17
   {-1,1,0},  //18
   {-1,-1,-1},//19

   {0,-1,1},  //20
   {0,-1,0},  //21
   {-1,0,1},  //22
   {-1,0,-1}, //23

   {-1,-1,0}, //24
   {-1,0,0},  //25
   {-1,-1,1}, //26
  };

const int8_t normal_idx_table[256] =
  {
   0,  1,  2,  3,  //4
   4,  5,  6,  3,  //8
   7,  6,  8,  6,  //12
   9,  6,  6,  6,  //16
   10, 11, 12, 12, //20
   13, 13, 1,  1,  //24
   1,  1,  1,  1,  //28
   1,  5,  1,  1,  //32
   15, 12, 12, 12, //36
   2,  12, 12, 7,  //40
   16, 2,  16, 2,  //44
   7,  2,  17, 2,  //48
   18, 12, 12, 12, //52
   14, 1,  1,  1,  //56
   19, 2,  2,  2,  //60
   3,  3,  3,  3,  //64
   14, 13, 4,  4,  //68
   20, 13, 4,  4,  //72
   21, 4,  4,  4,  //76
   21, 4,  19, 15, //80
   22, 13, 23, 1,  //84
   24, 13, 19, 1,  //88
   25, 4,  23, 15, //92
   24, 4,  19, 5,  //96
   25, 10, 16, 2,  //100
   21, 5,  17, 7,  //104
   19, 2,  23, 3,  //108
   24, 5,  17, 7,  //112
   21, 11, 16, 12, //116
   24, 13, 19, 11, //120
   25, 2,  23, 15, //124
   24, 26, 19, 1,  //128
   19, 1,  16, 3,  //132
   12, 5,  17, 6,  //136
   17, 1,  16, 3,  //140
   25, 5,  17, 6,  //144
   21, 11, 23, 12, //148
   24, 13, 19, 1,  //152
   25, 11, 23, 15, //156
   24, 20, 19, 2,  //160
   23, 1,  16, 3,  //164
   21, 5,  17, 6,  //168
   16, 1,  16, 3,  //172
   21, 5,  17, 6,  //176
   25, 11, 23, 12, //180
   24, 22, 19, 1,  //184
   25, 11, 23, 12, //188
   24, 26, 19, 2,  //192
   24, 1,  17, 3,  //196
   12, 4,  17, 5,  //200
   12, 1,  23, 3,  //204
   12, 5,  17, 9,  //208
   25, 11, 19, 18, //212
   24, 20, 19, 4,  //216
   25, 11, 19, 15, //220
   24, 20, 19, 4,  //224
   25, 1,  23, 3,  //228
   21, 5,  17, 7,  //232
   17, 1,  23, 3,  //236
   21, 5,  17, 7,  //240
   25, 10, 23, 18, //244
   24, 26, 19, 10, //248
   21, 11, 23, 15, //152
   24, 26, 19, 0   //156
  };



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
  bool is_voxel_active(int x, int y, int z);
  uint8_t get_adj_voxels(int in_x, int in_y, int in_z);
  void generate_mesh();
  void generate_cube(GLfloat x, GLfloat y, GLfloat z,
		     bool pos_x, bool neg_x,
		     bool pos_y, bool neg_y,
		     bool pos_z, bool neg_z);
  
  glm::vec3 get_normal_from_adj(uint8_t adj);

  enum AdjOffset{
		 NX_NY_NZ = (1<<0),  //1
		 NX_NY_PZ = (1<<1),  //2
		 NX_PY_NZ = (1<<2),  //4
		 NX_PY_PZ = (1<<3),  //8
		 PX_NY_NZ = (1<<4),  //16
		 PX_NY_PZ = (1<<5),  //32
		 PX_PY_NZ = (1<<6),  //64
		 PX_PY_PZ = (1<<7),  //128
		ALL_NEG_X = 0b00001111,
		ALL_NEG_Y = 0b00110011,
		ALL_NEG_Z = 0b01010101,
  };
   
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
