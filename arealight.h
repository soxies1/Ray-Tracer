#ifndef AREALIGHT_H
#define AREALIGHT_H

#endif // AREALIGHT_H
#pragma once
#include "object.h"

class AreaLight : public Object
{
public:
    AreaLight(vec3 A, vec3 B, vec3 C){
        a = A;
        b = B;
        c = C;
        vec3 CA = A - C;
        vec3 CB = B - C;
        for(int i = 0; i < 4; i ++){
            for(int j = 0; j < 4; j++){
                coords[i][j] = C +( CA * (i/4.0f) + CB * (j/4.0f));
                rands[i*4 + j] = ((float)(rand() % 400)/50.0f);
            }
        }
    }

    virtual bool Intersect(vec3 Origin, vec3 Direction,
                           float * t, vec3 * normal, vec3 * color, bool * reflective, bool * transparent)
    {

        return true;
    }

    vec3 pointOnLight(int i){

        vec3 A = a;
        vec3 B = b;
        vec3 C = c;
        int h = i/4;
        int g = i % 4;
        vec3 spot = coords[h][g];

        vec3 jitter(rands[i%16], rands[(i+1)%16], rands[(i+2)%16]);
        shufflerands();
        return spot + jitter;
        //return C + (CA * ((float)(rand() % 100)/100.0f)) + (CB * ((float)(rand() % 100)/100.0f));
        /*if(i<10)
        return C + (CA * ((float)(i/10.0f)) + (CB * ((float)(i/10.0f))));
        else{
            return C + (CA * 0.5f + (CB * ( (float)( (i-10) / 10.0f ) )));
        }*/
        //return C;
    }

    void shufflerands(){
        int start = rand()%16;
        for(int i = 0; i < 8; i++){
            rands[(start + i)%16] = rands[i];
        }
    }

    vec3 coords[4][4];
    float rands[16];

    vec3 a,b,c;
};
