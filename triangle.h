#ifndef TRIANGLE_H
#define TRIANGLE_H

#endif // TRIANGLE_H

#pragma once
#include "object.h"

class Triangle : public Object
{
public:

    Triangle(vec3 a_in, vec3 b_in, vec3 c_in, vec3 norm, vec3 color, bool is_reflective, bool is_transparent){
        a = a_in;
        b = b_in;
        c = c_in;
        NormalToPlane = norm;
        PlaneColor = color;
        Reflective = is_reflective;
        Transparent = is_transparent;
    }

    virtual bool Intersect(vec3 e, vec3 d,
                           float * t_out, vec3 * normal, vec3 * color, bool * reflective, bool * transparent)
    {
        mat3 AM(a.x - b.x , a.y - b.y, a.z - b.z, a.x - c.x, a.y - c.y, a.z - c.z, d.x, d.y, d.z );

        float A = determinant(AM);

        mat3 tM(a.x - b.x , a.y - b.y, a.z - b.z, a.x - c.x, a.y - c.y, a.z - c.z, a.x - e.x , a.y - e.y, a.z - e.z );
        float t = determinant(tM) / A;

        if(t < 0 || t > 99999) return false;


        mat3 gammaM(a.x - b.x , a.y - b.y, a.z - b.z, a.x - e.x, a.y - e.y, a.z - e.z, d.x, d.y, d.z );

        float gamma = determinant(gammaM) / A;


        if(gamma < 0 || gamma > 1) return false;


        mat3 betaM(a.x - e.x , a.y - e.y, a.z - e.z, a.x - c.x, a.y - c.y, a.z - c.z, d.x, d.y, d.z );

        float beta = determinant(betaM) / A;

        if(beta < 0 || beta > 1 - gamma) return false;

        *t_out = t;
        *normal = NormalToPlane;
        *color = PlaneColor;
        *transparent = Transparent;
        *reflective = Reflective;

        return true;


        /*float top = dot(Origin, NormalToPlane) - dot(PointOnPlane, NormalToPlane);
        float bot = - (dot(Direction,NormalToPlane));
        float temp = top / bot;
        if(temp > 0){
            *t = temp;
            *color = PlaneColor;
            *normal = NormalToPlane;
            return true;
        }else{
            return false;
        }*/
    }
    vec3 a,b,c;
    vec3 NormalToPlane;
    vec3 PlaneColor;

    bool Transparent;
    bool Reflective;
};
