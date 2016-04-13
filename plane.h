#pragma once
#include "object.h"

class Plane : public Object
{
public:

    Plane(Vector3 norm, Vector3 point, Vector3 color){
        NormalToPlane = norm;
        PointOnPlane = point;
        PlaneColor = color;
    }

    virtual bool Intersect(Vector3 Origin, Vector3 Direction,
                           float * t, Vector3 * normal, Vector3 * color)
    {
        float top = DotProduct(Origin, NormalToPlane) - DotProduct(PointOnPlane, NormalToPlane);
        float bot = - (DotProduct(Direction,NormalToPlane));
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

    Vector3 NormalToPlane;
    Vector3 PointOnPlane;
    Vector3 PlaneColor;
};
