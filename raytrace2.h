#pragma once
#include <math.h>
#include <tgmath.h>
#include <iostream>
#include <stdio.h>
#include <ctime>
#include <vector>

#include "Image.h"
#include "plane.h"
#include "triangle.h"
#include "sphere.h"
#include "light.h"
#include "arealight.h"
#include "object.h"
#include "common.h"
#include "boundingsphere.h"
#include "cube.h"

#define NUMSPHERES 4
#define NUMLIGHTS 2
#define PHONGPOWER 16

std::vector<Object *> ObjectList;

std::vector<Light *> LightList;

std::vector<AreaLight *> AreaLightList;

BoundingSphere SphereBound;

int refl_obj;

vec3 BackgroundColor(0,0,0);

float rand_table[16];

bool optimize = true; //18 with optimize true, 21 with false

//Compute Shading of Surface
Pixel Shadeing(vec3 SurfaceColor, float LightIntensity, vec3 Normal, vec3 Intersection, vec3 Direction)
{
    Pixel px;
    vec3 MaxC(0,0,0);
    vec3 final(0,0,0);
    int count=0;
    bool InShadow = false;
    //int siz = AreaLightList.size();
    bool InShade[1] = { 0 };
    float shade_coeff = 0.4;


    for(int i = 0; i < AreaLightList.size(); i++){
        //Soft Shadows
        for(int h = 0; h < 16; h++){

            //Lambertain
            vec3 L = AreaLightList[i]->pointOnLight(h) - Intersection;
            //vec3 L = Minus(LightList[0]->Point, Intersection);
            L = normalize(L);
            float I_Max = LightIntensity * MaxValue(0, dot(Normal,L));
            vec3 OutColor = SurfaceColor * I_Max;

            //Phong
            vec3 H = L + Direction;
            H = normalize(H);
            float S_Max = MaxValue(0, dot(Normal,H));
            //float hold = S_Max;
            S_Max = pow(S_Max, PHONGPOWER);
            //for(int j = 0; j < PHONGPOWER; j++){
            //    S_Max = S_Max * hold;
            //}

            S_Max = S_Max * LightIntensity;
            OutColor = OutColor + (vec3(200,200,200) * S_Max);
            SetColor(px, OutColor);
            MaxC = MaxColor(MaxC, OutColor);

            //Adjust instersection point to avoid black plague
            Intersection = Intersection + (L*0.002f);

            //Do Shadow
            //float t_min = 999999;
            //bool HasIntersection = false;
            bool x,y;
            x = y = false;
            //Intersect with the list of objects
            for (int k = 0; k < ObjectList.size() - 12; ++ k)
            {
                float t;
                vec3 normal;
                vec3 color;
                bool DoesIntersect = ObjectList[k]->Intersect(Intersection, L, &t, &normal, &color,&x,&y);
                if (DoesIntersect)
                {
                    InShade[i] = true;
                    InShadow = true;
                    if(y){
                        shade_coeff = .8f;
                    }
                    //SetColor(px, MultiplyScalar(OutColor,.2));
                    break;
                }
            }

            if(InShadow)
            for(int l = 0; l < AreaLightList.size(); l++){
                if(InShade[l]) MaxC = (MaxC * shade_coeff);
            }
            final += MaxC;
            count++;
        }
    }

    final /= count;
    SetColor(px,final);
    //SetColor(px,MaxC);

    return px;
}

bool in_refraction = false;

bool spheres_added = false;

void AddSpheres(){
    if(spheres_added) return;
    for(int i = 0; i < SphereBound.ObjectList.size(); i++){
        ObjectList.insert(ObjectList.begin(),SphereBound.ObjectList[i]);
    }
    spheres_added = true;

}

void RemoveSpheres(){
    if(!spheres_added) return;
    ObjectList.erase(ObjectList.begin(),ObjectList.begin() + SphereBound.ObjectList.size());
    spheres_added = false;
}

//Compute ray function solves intersect of objects from a point
Pixel ComputeRay(vec3 FromPoint, vec3 Direction, float LightIntensity, int j){
    Pixel px;

    float t_min = 999999;
    vec3 Normal_min;
    bool HasIntersection = false;
    vec3 color;
    vec3 color_min;
    int obj_hit;
    int k;
    bool is_trans, is_reflec;
    is_trans = is_reflec = false;

    //Bounding sphere
    if(optimize){
        float * trash;
        vec3 trash1, trash2;
        bool trash3,trash4;

        if(SphereBound.Intersect(FromPoint, Direction,trash, &trash1, &trash2, &trash3, &trash4)){
            AddSpheres();
        }
    }

    //Intersect with the list of objects
    for (k = 0; k < ObjectList.size(); ++ k)
    {
        float t;
        vec3 normal;
        bool * ref;
        bool * tra;
        bool DoesIntersect = ObjectList[k]->Intersect(FromPoint, Direction, &t, &normal, &color, ref, tra);
        if (DoesIntersect)
        {
            HasIntersection = true;
            if (t_min > t)
            {
                t_min = t;
                Normal_min = normal;
                color_min = color;
                obj_hit = k;
                is_trans = *tra;
                is_reflec = *ref;
            }
        }
    }
    if (HasIntersection)
    {
        if(obj_hit >= ObjectList.size() - 2){
            SetColor(px, vec3(255,255,255));
            return px;
        }
        vec3 Intersection = Direction * t_min;
        Intersection = Intersection + FromPoint;
        px = Shadeing(color_min, LightIntensity, Normal_min, Intersection, Direction);

        //If hit object is transparent/ refraction
        if(is_trans){
            vec3 N = Normal_min;
            vec3 D = Direction;
            vec3 res_t;
            float n = 1.00f; //air coeff
            float ng = 1.51f; //glass coeff

            if(in_refraction){
                float temp = n;
                n = ng;
                ng = temp;
            }
            in_refraction = !in_refraction;

            float root_val = 1 - ( ( (n*n) * (1 - dot(D, N)*dot(D,N))) / (ng*ng) );
            if( root_val < 0.0f ){ root_val = 0.0f;}

                 res_t = (n*(D - N*(dot(D,N)) )/ng) - N*(sqrt(root_val));

                vec3 new_intersection = Intersection + res_t*(0.002f);
                //in_refraction = !in_refraction;


                px = ComputeRay(new_intersection,normalize(res_t),LightIntensity,j);

                if(!in_refraction){
                    //vec3 N = Normal_min;
                    float TwoD_Dot_N = dot(Direction,N) * 2;
                    vec3 N_Time = N * TwoD_Dot_N;
                    vec3 R = Direction - N_Time;
                    R = normalize(R);
                    Pixel refl = ComputeRay(new_intersection, R, 0.5f, j);
                    px = AddColors(refl,px);
                }

        }

        //Make chosen object reflective
        if(is_reflec == true){
            //r = d - 2(d . n)n
            vec3 N = Normal_min;
            float TwoD_Dot_N = dot(Direction,N) * 2;
            vec3 N_Time = N * TwoD_Dot_N;
            vec3 R = Direction - N_Time;
            R = normalize(R);
            Pixel refl = ComputeRay(Intersection, R, 1.0, j);
            px = AddColors(refl,px);
        }
        //If Hit bottom/floor plane
        /*if(obj_hit == 2 || obj_hit == 3){
            //Give stripe texture
            vec3 stripecolor(120, 120, 120);
            Pixel h;
            if(sin(M_PI*j/8)>0){
                SetColor(h, stripecolor);
                px = MinusColors(px,h);
            }
        }*/
    }//if t > 0
    else //No Intersection, set background colour
    {
        SetColor(px, BackgroundColor);
    }
    if(optimize){
        RemoveSpheres();
    }
    return px;
}


//Aliasing with jittering
Pixel aliasedPixel(vec3 pixelPosition, vec3 Camera, float LightIntensity, int j){
    vec3 Direction1, Direction2, Direction3, Direction4;
    float rands[8];
    int posOrNeg = 0;

    for(int i = 0; i < 8; i++){
        posOrNeg = rand() % 2;
        rands[i] = (float)(rand() % 45) / 100.0f;
        if(posOrNeg == 1){
            rands[i] *= -1;
        }
    }

    //Section 1 math
    Direction1 = pixelPosition + vec3(-0.5 + rands[0], 0.5 + rands[1], 0);
    Direction1 = Direction1 -Camera;
    Direction1 = normalize(Direction1);
    Direction2 = pixelPosition + vec3(-0.5 + rands[2], -0.5 + rands[3], 0);
    Direction2 = Direction2 - Camera;
    Direction2 = normalize(Direction2);
    Direction3 = pixelPosition + vec3(0.5 + rands[4], 0.5 + rands[5], 0);
    Direction3 = Direction3 - Camera;
    Direction3 = normalize(Direction3);
    Direction4 = pixelPosition + vec3(0.5 + rands[6], -0.5 + rands[7], 0);
    Direction4 = Direction4 - Camera;
    Direction4 = normalize(Direction4);

    //Get Average of the 2x2 pixel colours
    return AveragePixel(ComputeRay(Camera, Direction1, LightIntensity, j),ComputeRay(Camera, Direction2, LightIntensity, j),
                      ComputeRay(Camera, Direction3, LightIntensity, j),ComputeRay(Camera, Direction4, LightIntensity, j));

}

//Sphere coordinate transform with matrix
void TransformSphere(Sphere * in){
    mat4 transform = translate(mat4(1.0f), vec3(200.0f,200.0f,200.0f));

    vec4 out = transform * vec4(in->Center, 1.0f);
    in->Center = vec3(out.x, out.y, out.z);
}

void initRandTable(){
    for(int i = 0; i < 16; i++){
        rand_table[i] = (float)( rand() % 100) / 100.0f;
    }
}

float BoundCenter(vec3 a, vec3 b, vec3 c, vec3 * out){
    vec3 hold((a.x+ b.x + c.x)/3,(a.y+ b.y + c.y)/3,(a.z+ b.z + c.z)/3);
    float val = 0;
    if(length(a-hold)>val) val = length(a-hold);
    if(length(b-hold)>val) val = length(b-hold);
    if(length(c-hold)>val) val = length(c-hold);
    *out = hold;
    return val + 81.0f;
}

void RayTraceImage(Image * pImage)
{
    //Declare Spheres
    Sphere One(vec3(-50.0f,-120.0f,20.0f), 80, vec3(20,20,20), true, false);
    TransformSphere(&One);
    Sphere Two(vec3(0.0f,-160.0f,-80.0f), 40, vec3(1,56,75) ,false, false);
    TransformSphere(&Two);
    Sphere Three(vec3(175.0f,-40.0f, 0.0f), 80, vec3(250,250,250),false, true);
    TransformSphere(&Three);
    //Sphere Four(vec3(200.0f,-150.0f,-40.0f), 50, vec3(80,186,168),false, false);
    //TransformSphere(&Four);

    Cube cube(C_C,C_LBN, C_LBF, C_LTN, C_LTF, C_RTF, C_RBN, C_RBF, C_RTN, vec3(29,70, 100), false, false);

    //Add to object List

    vec3 bound_center;
    float bound_radius = BoundCenter(vec3(150.0f,80.0f,220.0f),vec3(200.0f,40.0f,120.0f),vec3(375.0f,160.0f, 200.0f), &bound_center);
    std::vector<Object *> BoundList;
    BoundList.push_back(&One);
    BoundList.push_back(&Two);
    BoundList.push_back(&Three);

    SphereBound = BoundingSphere(bound_center, bound_radius,BoundList);

    ObjectList.push_back(&cube);

    if(!optimize){
    ObjectList.push_back(&One);
    ObjectList.push_back(&Two);
    ObjectList.push_back(&Three);
    }

    AreaLight AreaLight1(L_RF, L_LN, L_RN);
    //AreaLight AreaLight2(L_LF, L_RF, L_LN);

    AreaLightList.push_back(&AreaLight1);
    //AreaLightList.push_back(&AreaLight2);

    //Define Camera Position
    vec3 Camera(256, 256, -300);
    float LightIntensity = 1.0;


    Triangle Bottom1(LBN, LBF, RBF, vec3(0,1,0), vec3(252,251,250),false, false);
    Triangle Bottom2(LBN, RBN, RBF, vec3(0,1,0), vec3(252,251,250),false, false);

    Triangle Left1(LBN, LBF, LTF, vec3(1,0,0), vec3(136,141,147),false, false);
    Triangle Left2(LBN, LTN, LTF, vec3(1,0,0), vec3(136,141,147),false, false);

    Triangle Right1(RBN, RBF, RTF, vec3(-1,0,0), vec3(213,77,48),false, false);
    Triangle Right2(RBN, RTN, RTF, vec3(-1,0,0), vec3(213,77,48),false, false);

    Triangle Top1(RTN, LTF, RTF, vec3(0,-1,0), vec3(10,10,10),false, false);
    Triangle Top2(LTN, RTN, RTF, vec3(0,-1,0), vec3(10,10,10),false, false);

    Triangle Back1(LBF, LTF, RBF, vec3(0,0,-1), vec3(136,141,147),false, false);
    Triangle Back2(RBF, RTF, LTF, vec3(0,0,-1), vec3(136,141,147),false, false);

//printf("Got here 0");
    //Add Planes to object list
    ObjectList.push_back(&Bottom1);
    ObjectList.push_back(&Bottom2);
    ObjectList.push_back(&Left1);
    ObjectList.push_back(&Left2);
    ObjectList.push_back(&Right1);
    ObjectList.push_back(&Right2);
    ObjectList.push_back(&Top1);
    ObjectList.push_back(&Top2);
    ObjectList.push_back(&Back1);
    ObjectList.push_back(&Back2);

    Triangle UpperLight1(L_LN, L_LF, L_RN, vec3(0,-1,0), vec3(255,255,255),false, false);
    Triangle UpperLight2(L_LF, L_RF, L_RN, vec3(0,-1,0), vec3(255,255,255),false, false);

    ObjectList.push_back(&UpperLight1);
    ObjectList.push_back(&UpperLight2);

    srand(time(NULL));
    initRandTable();

    for (int i = 0; i < 512; ++ i)
        for (int j = 0; j < 512; ++j)
        {
            //Compute O +tD
            vec3 PixelPosition((float)i, (float)j, 0);

            //For 4 rays per pixel anit-aliasing done
            //(*pImage)(i, 511-j) = AveragePixel(aliasedPixel(PixelPosition, Camera, LightIntensity, j),aliasedPixel(PixelPosition, Camera, LightIntensity, j),aliasedPixel(PixelPosition, Camera, LightIntensity, j),aliasedPixel(PixelPosition, Camera, LightIntensity, j));
            (*pImage)(i, 511-j) = aliasedPixel(PixelPosition, Camera, LightIntensity, j);
            //Pixel out = ComputeRay(PixelPosition, Direction, LightIntensity,j);

        }//End 512x512 For loop

}//End RayTraceImage Function
