#pragma once
#include "object.h"

class Plane : public Object
{
public:

    Plane(vec3 norm, vec3 point, vec3 color){
        NormalToPlane = norm;
        PointOnPlane = point;
        PlaneColor = color;
    }

    virtual bool Intersect(vec3 Origin, vec3 Direction,
                           float * t, vec3 * normal, vec3 * color, bool * reflective, bool * transparent)
    {
        float top = dot(Origin, NormalToPlane) - dot(PointOnPlane, NormalToPlane);
        float bot = - (dot(Direction,NormalToPlane));
        float temp = top / bot;
        if(temp > 0){
            *t = temp;
            *color = PlaneColor;
            *normal = NormalToPlane;
            return true;
        }else{
            return false;
        }
    }

    vec3 NormalToPlane;
    vec3 PointOnPlane;
    vec3 PlaneColor;
};
