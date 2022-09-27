#include <iostream>

#include "render/World.h"

int main(){

    std::vector<Object*> vo;
    std::vector<Light*> vl;
    
    Camera* c = new Camera(
        4, // near
        60, // fov
        600, // w
        400, // h
        glm::vec3(3,5,30), // eye
        glm::vec3(0,0,0), // center
        glm::vec3(0,-1,0) // up
    );


    Object* obj = new Sphere(
        {-8,9,-2}, // centroid
        {1, 1 , 0}, //color
        4, // radious
        0.9, // kd
        0.5, // ks
        8,// n
        23
    );


    Object* obj1 = new Sphere(
        {3,8,-6}, // centroid
        {0, 0.5 , 1}, //color
        4, // radious
        0.9,
        2,
        3,
        0 // idr
    );

    Object* pPlane = new Plane(
        {1,0,1}, // color
        {0,1,0}, //N
        2, // d
        0.7,
        0,
        1,
        0
    );


    Light*  l = new Light{
        {10,30,20}, // pos
        {1,1,1} // color
    };
    


    // for(int i = 0; i < 10; i++){
    //     for(int j = 0; j < 4; j++){
    //         vo.push_back( new Sphere(
    //             {0 + i  * 5, 5+ j * 1,0}, // centroid
    //             {0, 1 , 1}, //color
    //             0.7, // radious
    //             0.9,
    //             0.1,
    //             32
    //         ));
    //     }
    // }
    vo.push_back(obj);
    vo.push_back(obj1);
    
    vo.push_back(pPlane); 
    vl.push_back(l);

    World world(c, vo, vl);

    world.insertLight(l);
    
    
    world.render();

    delete c;

    return 0;
}