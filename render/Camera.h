#ifndef _CAMERA_H_
#define _CAMERA_H_


#include <glm/glm.hpp>
#include <math.h>


using Point  = glm::vec3;

struct Ray{
    Point o,d;
};


class Camera{

    private:

    float f,w, h, a,b;
    Point pos; // position of camera   
    Point xe,ye,ze; // unitary vectors that define the camera

    friend class World;

    public:

        Camera(float near, float fov, float w, float h, Point eye, Point center, Point up): f(near), pos(eye), w(w), h(h){
            a = 2 * f * tan(fov* M_PI/180 /2); 
            b = w/h * a;
            ze  =  glm::normalize(eye - center);
            xe = glm::normalize( glm::cross(up, ze));
            ye = glm::cross(ze, xe);
        }
        
        Ray raycast(float x, float  y){
            Point d = -f * ze + a * (y/h - 0.5f) * ye + b * (x / w - 0.5f) * xe;
            return Ray{pos, d};
        }

        glm::vec3 calculateColor(const Ray &ray, std::vector<class Object *> objects, std::vector<class Light *> lights, class Light *ambient, int prof){
            glm::vec3 result(1.0f, 1.0f, 1.0f);
            
        }
};



#endif