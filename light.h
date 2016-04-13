#pragma once
#include "object.h"

class Light : public Object
{
public:
    Light(Vector3 location){
        Point = location;
    }

    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t, Vector3 * normal, Vector3 * color)
    {
        return true;
    }

    Vector3 Point;
};
