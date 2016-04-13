#pragma once
#include "object.h"

class Cube : public Object
{
public:

    Cube(vec3 center, vec3 LBN_in, vec3 LBF_in, vec3 LTN_in,vec3 LTF_in, vec3 RTF_in, vec3 RBN_in, vec3 RBF_in, vec3 RTN_in, vec3 color, bool is_reflective, bool is_transparent){

        Triangle Bottom1(LBN_in, LBF_in, RBF_in, vec3(0,-1,0), color,is_reflective, is_transparent);
        Triangle Bottom2(LBN_in, RBN_in, RBF_in, vec3(0,-1,0), color,is_reflective, is_transparent);

        Triangle Left1(LBN_in, LBF_in, LTF_in, vec3(-1,0,0), color,is_reflective, is_transparent);
        Triangle Left2(LBN_in, LTN_in, LTF_in, vec3(-1,0,0), color,is_reflective, is_transparent);

        Triangle Right1(RBN_in, RBF_in, RTF_in, vec3(1,0,0), color,is_reflective, is_transparent);
        Triangle Right2(RBN_in, RTN_in, RTF_in, vec3(1,0,0), color,is_reflective, is_transparent);

        Triangle Top1(RTN_in, LTF_in, RTF_in, vec3(0,1,0), color,is_reflective, is_transparent);
        Triangle Top2(LTN_in, RTN_in, LTF_in, vec3(0,1,0), color,is_reflective, is_transparent);

        Triangle Back1(LBF_in, LTF_in, RBF_in, vec3(0,0,1), color,is_reflective, is_transparent);
        Triangle Back2(RBF_in, RTF_in, LTF_in, vec3(0,0,1), color,is_reflective, is_transparent);

        Triangle Front1(LBN_in, LTN_in, RBN_in, vec3(0,0,-1), color,is_reflective, is_transparent);
        Triangle Front2(RBN_in, RTN_in, LTN_in, vec3(0,0,-1), color,is_reflective, is_transparent);

    //printf("Got here 0");
        //Add Planes to object list
        Triangles.push_back(Bottom1);
        Triangles.push_back(Bottom2);
        Triangles.push_back(Left1);
        Triangles.push_back(Left2);
        Triangles.push_back(Right1);
        Triangles.push_back(Right2);
        Triangles.push_back(Top1);
        Triangles.push_back(Top2);
        Triangles.push_back(Back1);
        Triangles.push_back(Back2);
        Triangles.push_back(Front1);
        Triangles.push_back(Front2);

        PlaneColor = color;
        Reflective = is_reflective;
        Transparent = is_transparent;
        Sphere bounds( center, length(center - LBN_in)+1.0f, vec3(0,0,0), false,false );
        bound.push_back(bounds);
    }

    virtual bool Intersect(vec3 e, vec3 d,
                           float * t_out, vec3 * normal, vec3 * color, bool * reflective, bool * transparent)
    {
        float temp1;
        vec3 temp_norm1;
        vec3 temp_color1;
        bool hold_trans1, hold_refl1;

        if(!(bound[0].Intersect(e,d,&temp1, &temp_norm1, &temp_color1, &hold_refl1, &hold_trans1))){
            return false;
        }

        float t_min = 9999999;
        float temp;
        vec3 temp_norm;
        vec3 temp_color;
        vec3 normal_min;
        bool hold_trans, hold_refl;
        bool does_inter = false;
        for(int i = 0; i < Triangles.size(); i ++){
            if(Triangles[i].NormalToPlane.z == 1 || Triangles[i].NormalToPlane.y == -1) continue;
            if(Triangles[i].Intersect(e,d,&temp,&temp_norm, &temp_color, &hold_refl, &hold_trans)){
                if(temp < t_min){
                    t_min = temp;
                    normal_min = temp_norm;
                    does_inter = true;
                }
            }
        }
        if(does_inter){
            *t_out = t_min;
            *normal = normal_min;
            *color = PlaneColor;
            *transparent = Transparent;
            *reflective = Reflective;
            return true;
        }else{
            return false;
        }

    }

    std::vector<Sphere> bound;
    vec3 LBN, LBF, LTN, LTF, RBN, RBF, RTN, RTF;
    vec3 PlaneColor;
    std::vector<Triangle> Triangles;
    bool Transparent;
    bool Reflective;
};
