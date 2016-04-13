#pragma once
#include <math.h>

#include <glm/glm.hpp>

using namespace glm;

#include <glm/gtc/matrix_transform.hpp>

#define PI 3.14159265

//Define room dimensions
#define NEAR 0
#define FAR 300
#define RIGHT 512
#define LEFT 0
#define TOP 512
#define BOTTOM 0

vec3 LBN(LEFT, BOTTOM, NEAR);
vec3 LTN(LEFT, TOP, NEAR);
vec3 LBF(LEFT, BOTTOM, FAR);
vec3 LTF(LEFT, TOP, FAR);

vec3 RBN(RIGHT, BOTTOM, NEAR);
vec3 RTN(RIGHT, TOP, NEAR);
vec3 RBF(RIGHT, BOTTOM, FAR);
vec3 RTF(RIGHT, TOP, FAR);

//Define Light dimensions
#define LIGHT_HEIGHT 500
#define LIGHT_CENTER_Z FAR/2 - 50
#define LIGHT_CENTER_X RIGHT/2
#define LIGHT_NEAR LIGHT_CENTER_Z - 60
#define LIGHT_FAR LIGHT_CENTER_Z + 60
#define LIGHT_RIGHT LIGHT_CENTER_X + 80
#define LIGHT_LEFT LIGHT_CENTER_X - 80

vec3 L_LN(LIGHT_LEFT, LIGHT_HEIGHT, LIGHT_NEAR);
vec3 L_LF(LIGHT_LEFT, LIGHT_HEIGHT, LIGHT_FAR);
vec3 L_RN(LIGHT_RIGHT, LIGHT_HEIGHT, LIGHT_NEAR);
vec3 L_RF(LIGHT_RIGHT, LIGHT_HEIGHT, LIGHT_FAR);

#define CUBE_W 95
#define CUBE_L 325
#define CUBE_R CUBE_L + CUBE_W
#define CUBE_B 10
#define CUBE_T CUBE_B + CUBE_W
#define CUBE_N 150
#define CUBE_F CUBE_N + CUBE_W


vec3 C_LBN(CUBE_L, CUBE_B, CUBE_N);
vec3 C_LTN(CUBE_L, CUBE_T, CUBE_N);
vec3 C_LBF(CUBE_L, CUBE_B, CUBE_F);
vec3 C_LTF(CUBE_L, CUBE_T, CUBE_F);

vec3 C_RBN(CUBE_R, CUBE_B, CUBE_N);
vec3 C_RTN(CUBE_R, CUBE_T, CUBE_N);
vec3 C_RBF(CUBE_R, CUBE_B, CUBE_F);
vec3 C_RTF(CUBE_R, CUBE_T, CUBE_F);

vec3 C_C(CUBE_L + CUBE_W/2, CUBE_B + CUBE_W/2, CUBE_N + CUBE_W/2);




//Converts Vector color representation to px
void SetColor(Pixel & px, vec3 CalculatedColor)
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
vec3 MaxColor(vec3 MColor, vec3 ResultColor){
    int one = MColor.x + MColor.y + MColor.z;
    int two = ResultColor.x + ResultColor.y + ResultColor.z;
    if (one > two){
        return MColor;
    }else{
        return ResultColor;
    }
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

//Returns Average Color of 4 Pixels
Pixel AveragePixel(Pixel one, Pixel two, Pixel three, Pixel four){
    Pixel x;
    x.R = (one.R + two.R + three.R + four.R )/4;
    x.G = (one.G + two.G + three.G + four.G )/4;
    x.B = (one.B + two.B + three.B + four.B )/4;
    return x;
}//End Average Pixel
