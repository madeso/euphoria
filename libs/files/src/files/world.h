#pragma once

#include "io/json.h"

namespace eu::files::world
{
  struct vec2f
  {
    float x;
    float y;
  };

  struct Property
  {
    std::string name;

    // only one is valid
    std::optional<vec2f> vec2f_value;
  };

  struct Object
  {
    std::string template_name;
    std::vector<Property> properties;
  };

  struct World
  {
    std::vector<Object> objects;
  };

  JSON_PARSE_FUNC(World);
}
