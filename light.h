#pragma once
#include "object.h"

class Light : public Object
{
public:
    Light(vec3 location){
        Point = location;
    }

    virtual bool Intersect(vec3 Origin, vec3 Direction,
                           float * t, vec3 * normal, vec3 * color, bool * reflective, bool * transparent)
    {
        return true;
    }

    vec3 Point;
};
