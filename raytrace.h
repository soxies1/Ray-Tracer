#pragma once
#include <math.h>
#include <tgmath.h>
#include <iostream>
#include <stdio.h>
#include <ctime>
#include <vector>

#include "Image.h"
#include "plane.h"
#include "sphere.h"
#include "light.h"
#include "object.h"
#include "common.h"

#define NUMSPHERES 4
#define NUMLIGHTS 2
#define PHONGPOWER 2

std::vector<Object *> ObjectList;

std::vector<Light *> LightList;

int refl_obj;

Vector3 BackgroundColor(255,255,255);

//Converts Vector color representation to px
void SetColor(Pixel & px, Vector3 CalculatedColor)
{
    if (CalculatedColor.x < 0) px.R = 0;
    else if (CalculatedColor.x > 255) px.R = 255;
    else px.R = (unsigned char)CalculatedColor.x;

    if (CalculatedColor.y < 0) px.G = 0;
    else if (CalculatedColor.y > 255) px.G = 255;
    else px.G = (unsigned char)CalculatedColor.y;

    if (CalculatedColor.z < 0) px.B = 0;
    else if (CalculatedColor.z > 255) px.B = 255;
    else px.B = (unsigned char)CalculatedColor.z;

    px.A = 255;
}

//Return the higher float value
float MaxValue(float zero, float N_Dot_L){
    if (zero >= N_Dot_L) return zero;
    else return N_Dot_L;
}

//Return the color thats lighter
Vector3 MaxColor(Vector3 MColor, Vector3 ResultColor){
    int one = MColor.x + MColor.y + MColor.z;
    int two = ResultColor.x + ResultColor.y + ResultColor.z;
    if (one > two){
        return MColor;
    }else{
        return ResultColor;
    }
}

//Compute Shading of Surface
Pixel Shadeing(Vector3 SurfaceColor, float LightIntensity, Vector3 Normal, Vector3 Intersection, Vector3 Direction)
{
    Pixel px;
    Vector3 MaxC(0,0,0);
    bool InShadow = false;
    bool InShade[NUMLIGHTS] = { 0 };


    for(int i = 0; i < LightList.size(); i++){
        //Lambertain
        Vector3 L = Minus(LightList[i]->Point, Intersection);
        //Vector3 L = Minus(LightList[0]->Point, Intersection);
        L = Normalize(L);
        float I_Max = LightIntensity * MaxValue(0, DotProduct(Normal,L));
        Vector3 OutColor = MultiplyScalar(SurfaceColor,I_Max);

        //Phong
        Vector3 H = Add(L, Direction);
        H = Normalize(H);
        float S_Max = MaxValue(0, DotProduct(Normal,H));
        float hold = S_Max;
        for(int j = 0; j < PHONGPOWER; j++){
            S_Max = S_Max * hold;
        }

        S_Max = S_Max * LightIntensity;
        OutColor = Add(OutColor, MultiplyScalar(Vector3(200,200,200), S_Max));
        SetColor(px, OutColor);
        MaxC = MaxColor(MaxC, OutColor);

        //Adjust instersection point to avoid black plague
        Intersection = Add(Intersection, MultiplyScalar(L,.2));

        //Do Shadow
        float t_min = 999999;
        bool HasIntersection = false;
        //Intersect with the list of objects
        for (int k = 0; k < ObjectList.size() - 5; ++ k)
        {
            float t;
            Vector3 normal;
            Vector3 color;
            bool DoesIntersect = ObjectList[k]->Intersect(Intersection, L, &t, &normal, &color);
            if (DoesIntersect)
            {
                InShade[i] = true;
                InShadow = true;
                //SetColor(px, MultiplyScalar(OutColor,.2));
                break;
            }
        }
    }
    if(InShadow)
    for(int l = 0; l < NUMLIGHTS; l++){
        if(InShade[l]) MaxC = MultiplyScalar(MaxC, .6);
    }
    //if(InShadow) SetColor(px, MultiplyScalar(MaxC,.2));
    //else
    SetColor(px,MaxC);

    return px;
}

//Add Pixel colors and prevent going higher than 255
Pixel AddColors(Pixel px, Pixel refl){
    int x,y,z;
    Pixel a;
    x = px.R + refl.R;
    if(x > 255) x = 255;
    y = px.G + refl.G;
    if(y > 255) y = 255;
    z = px.B + refl.B;
    if(z > 255) z = 255;
    a.R = x;
    a.G = y;
    a.B = z;
    return a;
}

//Minus Pixel colors and prevent going lower than 0
Pixel MinusColors(Pixel px, Pixel refl){
    int x,y,z;
    Pixel a;
    x = px.R - refl.R;
    if(x < 0) x = 0;
    y = px.G - refl.G;
    if(y < 0) y = 0;
    z = px.B - refl.B;
    if(z < 0) z = 0;
    a.R = x;
    a.G = y;
    a.B = z;
    return a;
}

//Compute ray function solves intersect of objects from a point
Pixel ComputeRay(Vector3 FromPoint, Vector3 Direction, float LightIntensity, int j){
    Pixel px;

    float t_min = 999999;
    Vector3 Normal_min;
    bool HasIntersection = false;
    Vector3 color;
    Vector3 color_min;
    int obj_hit;
    int k;

    //Intersect with the list of objects
    for (k = 0; k < ObjectList.size(); ++ k)
    {
        float t;
        Vector3 normal;
        bool DoesIntersect = ObjectList[k]->Intersect(FromPoint, Direction, &t, &normal, &color);
        if (DoesIntersect)
        {
            HasIntersection = true;
            if (t_min > t)
            {
                t_min = t;
                Normal_min = normal;
                color_min = color;
                obj_hit = k;
            }
        }
    }
    if (HasIntersection)
    {
        Vector3 Intersection = MultiplyScalar(Direction, t_min);
        Intersection = Add(Intersection, FromPoint);
        px = Shadeing(color_min, LightIntensity, Normal_min, Intersection, Direction);

        //Make chosen object reflective
        if(obj_hit == refl_obj ){
            //r = d - 2(d . n)n
            Vector3 N = Normal_min;
            float TwoD_Dot_N = DotProduct(Direction,N) * 2;
            Vector3 N_Time = MultiplyScalar(N, TwoD_Dot_N);
            Vector3 R = Minus(Direction, N_Time);
            R = Normalize(R);
            Pixel refl = ComputeRay(Intersection, R, 0.9, j);
            px = AddColors(refl,px);
        }
        //If Hit bottom/floor plane
        if(obj_hit == 4){
            //Give stripe texture
            Vector3 stripecolor(120, 120, 120);
            Pixel h;
            if(sin(M_PI*j/8)>0){
                SetColor(h, stripecolor);
                px = MinusColors(px,h);
            }
        }
    }//if t > 0
    else //No Intersection, set background colour
    {
        SetColor(px, BackgroundColor);
    }
    return px;
}

//Returns Average Color of 4 Pixels
Pixel AveragePixel(Pixel one, Pixel two, Pixel three, Pixel four){
    Pixel x;
    x.R = (one.R + two.R + three.R + four.R )/4;
    x.G = (one.G + two.G + three.G + four.G )/4;
    x.B = (one.B + two.B + three.B + four.B )/4;
    return x;
}//End Average Pixel


void RayTraceImage(Image * pImage)
{
    //Declare Spheres
    Sphere One(Vector3(256,256,170), 80, Vector3(20,20,20));
    Sphere Two(Vector3(200,40,120), 40, Vector3(120,170,210));
    Sphere Three(Vector3(300,40,100), 38, Vector3(142,39,38));
    Sphere Four(Vector3(400,50,160), 50, Vector3(80,186,168));

    //Add to object List
    ObjectList.push_back(&One);
    ObjectList.push_back(&Two);
    ObjectList.push_back(&Three);
    ObjectList.push_back(&Four);

    //Create and Push Lights
    Light Light1(Vector3(128,128,0));
    LightList.push_back(&Light1);
    Light Light2(Vector3(210,256,0));
    LightList.push_back(&Light2);

    //Define Camera Position
    Vector3 Camera(256, 256, -200);
    float LightIntensity = 1.0;

    //Define the Planes that make up the box
    Plane Bottom(Vector3(0,1,0), Vector3(256,0,0), Vector3(252,251,250));
    Plane Left(Vector3(1,0,0), Vector3(0,256,0), Vector3(136,141,147));
    Plane Right(Vector3(-1,0,0), Vector3(512,256,0), Vector3(213,77,48));
    Plane Top(Vector3(0,-1,0), Vector3(256,512,0), Vector3(1,56,75));
    Plane Back(Vector3(0,0,-1), Vector3(0,256,220), Vector3(136,141,147));

    //Add Planes to object list
    ObjectList.push_back(&Bottom);
    ObjectList.push_back(&Left);
    ObjectList.push_back(&Right);
    ObjectList.push_back(&Top);
    ObjectList.push_back(&Back);

    //Make Random Object Reflective
    /*
    srand(time(NULL));
    refl_obj = rand() % (ObjectList.size() - 1);
    */

    //Default Sphere relflective
    refl_obj = 0;

    for (int i = 0; i < 512; ++ i)
        for (int j = 0; j < 512; ++j)
        {
            Pixel px;

            //Compute O +tD
            Vector3 PixelPosition((float)i, (float)j, 0);

            //Vector3 Direction = Minus(PixelPosition, Camera);
            //Direction = Normalize(Direction);

            //For 16 rays per pixel anit-aliasing done, doing each 2x2 section
            Pixel Section1, Section2, Section3, Section4;

            Vector3 Direction1, Direction2, Direction3, Direction4;

            //Section 1 math
            Direction1 = Add(PixelPosition,Vector3(-0.375, 0.375, 0));
            Direction1 = Minus(Direction1,Camera);
            Direction1 = Normalize(Direction1);
            Direction2 = Add(PixelPosition,Vector3(-0.125, 0.375, 0));
            Direction2 = Minus(Direction2,Camera);
            Direction2 = Normalize(Direction2);
            Direction3 = Add(PixelPosition,Vector3(-0.375, 0.125, 0));
            Direction3 = Minus(Direction3,Camera);
            Direction3 = Normalize(Direction3);
            Direction4 = Add(PixelPosition,Vector3(-0.125, 0.125, 0));
            Direction4 = Minus(Direction4,Camera);
            Direction4 = Normalize(Direction4);

            //Get Average of the 2x2 pixel colours
            Section1 = AveragePixel(ComputeRay(Camera, Direction1, LightIntensity, j),ComputeRay(Camera, Direction2, LightIntensity, j),
                              ComputeRay(Camera, Direction3, LightIntensity, j),ComputeRay(Camera, Direction4, LightIntensity, j));

            //Section2 Math
            Direction1 = Add(PixelPosition,Vector3(0.125, 0.375, 0));
            Direction1 = Minus(Direction1,Camera);
            Direction1 = Normalize(Direction1);
            Direction2 = Add(PixelPosition,Vector3(0.375, 0.375, 0));
            Direction2 = Minus(Direction2,Camera);
            Direction2 = Normalize(Direction2);
            Direction3 = Add(PixelPosition,Vector3(0.125, 0.125, 0));
            Direction3 = Minus(Direction3,Camera);
            Direction3 = Normalize(Direction3);
            Direction4 = Add(PixelPosition,Vector3(0.375, 0.125, 0));
            Direction4 = Minus(Direction4,Camera);
            Direction4 = Normalize(Direction4);

            //Get Average of the 2x2 pixel colours
            Section2 = AveragePixel(ComputeRay(Camera, Direction1, LightIntensity, j),ComputeRay(Camera, Direction2, LightIntensity, j),
                              ComputeRay(Camera, Direction3, LightIntensity, j),ComputeRay(Camera, Direction4, LightIntensity, j));

            //Section 3 Math
            Direction1 = Add(PixelPosition,Vector3(-0.375, -0.125, 0));
            Direction1 = Minus(Direction1,Camera);
            Direction1 = Normalize(Direction1);
            Direction2 = Add(PixelPosition,Vector3(-0.125, -0.125, 0));
            Direction2 = Minus(Direction2,Camera);
            Direction2 = Normalize(Direction2);
            Direction3 = Add(PixelPosition,Vector3(-0.375, -0.375, 0));
            Direction3 = Minus(Direction3,Camera);
            Direction3 = Normalize(Direction3);
            Direction4 = Add(PixelPosition,Vector3(-0.125, -0.375, 0));
            Direction4 = Minus(Direction4,Camera);
            Direction4 = Normalize(Direction4);

            //Get Average of the 2x2 pixel colours
            Section3 = AveragePixel(ComputeRay(Camera, Direction1, LightIntensity, j),ComputeRay(Camera, Direction2, LightIntensity, j),
                              ComputeRay(Camera, Direction3, LightIntensity, j),ComputeRay(Camera, Direction4, LightIntensity, j));

            //Section4 Math
            Direction1 = Add(PixelPosition,Vector3(0.125, -0.125, 0));
            Direction1 = Minus(Direction1,Camera);
            Direction1 = Normalize(Direction1);
            Direction2 = Add(PixelPosition,Vector3(0.375, -0.125, 0));
            Direction2 = Minus(Direction2,Camera);
            Direction2 = Normalize(Direction2);
            Direction3 = Add(PixelPosition,Vector3(0.125, -0.375, 0));
            Direction3 = Minus(Direction3,Camera);
            Direction3 = Normalize(Direction3);
            Direction4 = Add(PixelPosition,Vector3(0.375, -0.375, 0));
            Direction4 = Minus(Direction4,Camera);
            Direction4 = Normalize(Direction4);

            //Get Average of the 2x2 pixel colours
            Section4 = AveragePixel(ComputeRay(Camera, Direction1, LightIntensity, j),ComputeRay(Camera, Direction2, LightIntensity, j),
                              ComputeRay(Camera, Direction3, LightIntensity, j),ComputeRay(Camera, Direction4, LightIntensity, j));

            //Get Average of the 4 section colours
            (*pImage)(i, 511-j) = AveragePixel(Section1,Section2, Section3, Section4);

        }//End 512x512 For loop

}//End RayTraceImage Function
