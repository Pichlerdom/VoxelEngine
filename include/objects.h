#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <stdint.h>

#define GL3_PROTOTYPE 1
#include <GL/glew.h>


#define FLOATS_PER_POINT 3
#define FLOATS_PER_COLOR 4

#define POINTS_CUBE 12

const GLfloat cube[POINTS_CUBE][FLOATS_PER_POINT] = {
    {  0.2,  0.2,  0.5 }, // Top right
    { -0.2,  0.2,  0.5 }, // Top left
    {  0.0,  0.0,  0.5 }, // Center

    {  0.2,  0.2,  0.5 }, // Top right
    {  0.2, -0.2,  0.5 }, // Bottom right 
    {  0.0,  0.0,  0.5 }, // Center

    { -0.2, -0.2,  0.5 }, // Bottom left
    {  0.2, -0.2,  0.5 }, // Bottom right 
    {  0.0,  0.0,  0.5 }, // Center

    { -0.2, -0.2,  0.5 }, // Bottom left
    { -0.2,  0.2,  0.5 }, // Top left
    {  0.0,  0.0,  0.5 }, // Center
};

const GLfloat cube_colors[POINTS_CUBE][FLOATS_PER_COLOR] = {
    { 0.5, 0.5, 0.5, 1.0f }, // Top right
    { 0.5, 0.5, 0.5, 1.0f }, // Bottom right 
    { 0.0, 0.0, 0.0, 1.0f }, // Center

    { 0.5, 0.5, 0.5, 1.0f }, // Top left
    { 0.5, 0.5, 0.5, 1.0f }, // Top right
    { 0.0, 0.0, 0.0, 1.0f }, // Center

    { 0.5, 0.5, 0.5, 1.0f }, // Bottom left
    { 0.5, 0.5, 0.5, 1.0f }, // Bottom right 
    { 0.0, 0.0, 0.0, 1.0f }, // Center

    { 0.5, 0.5, 0.5, 1.0f }, // Bottom left
    { 0.5, 0.5, 0.5, 1.0f }, // Top left
    { 0.0, 0.0, 0.0, 1.0f }, // Center
};
#endif 
