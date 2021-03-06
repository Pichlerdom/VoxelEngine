#include "event_handler.h"
#include <glm/glm.hpp>

class Math
{
 public:
  // Returns a simple axis which tells us which direction to move the object in
  static glm::vec3 GetAxis(const EventHandler &handler)
  {
    glm::vec3 axis(0.0f);
    
    if (handler.IsKeyDown(SDLK_UP))
      axis.y = -1.0f;

    if (handler.IsKeyDown(SDLK_DOWN))
      axis.y = 1.0f;

    if (handler.IsKeyDown(SDLK_LEFT))
      axis.x = -1.0f;

    if (handler.IsKeyDown(SDLK_RIGHT))
      axis.x = 1.0f;

    if (handler.IsKeyDown(SDLK_w))
      axis.z = -1.0f;

    if (handler.IsKeyDown(SDLK_s))
      axis.z = 1.0f;

    return axis;
  }
};
