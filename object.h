#pragma once
#include "common.h"

class Object
{
public:
    virtual bool Intersect(vec3 Origin, vec3 Direction,
                           float * t, vec3 * normal, vec3 * color, bool * reflective, bool * transparent) = 0;

};
