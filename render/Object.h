#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "World.h"
#include <glm/vec3.hpp>
#include <cmath>


using Point = glm::vec3;


struct Ray{
    Point o,d;
};


class Object{

    private:
        float kd = 0.9 , ks = 0.5, n;
        glm::vec3 color;

    public:
        float idr = 0;

        Object(): color(255,0,0){
        } 

        Object(glm::vec3 cl, float kd, float ks, float n, float idr): color(cl), kd(kd), ks(ks), n(n), idr(idr) {

        }

        float getKd(){ return kd;}
        float getKs(){ return ks;}
        float getn(){return n;}
        float getIdr(){return idr;}

        

        glm::vec3& get_color(){
            return this->color;
        }
        
        virtual bool intersect(const Ray&, float&, glm::vec3 &) = 0;
};

class Sphere: public Object{
    private:
    
    public:

        
        Point cen;
        float r;


        Sphere(): Object(), cen(0,0,0){
            r  = 2;
        }

        Sphere(Point c, Point color, float r, float kd = 0.9, float ks = 0.5, float n  = 5.f, float idr = 0): Object(color,kd,ks,n, idr), cen(c), r(r){
        } 

        bool intersect(const Ray& ray, float& t, glm::vec3 &normal) override{

            auto oc = ray.o - cen;

            auto a = glm::dot(ray.d , ray.d);
            auto b = 2.0f * glm::dot(ray.d , oc );
            auto c = glm::dot( oc , oc)  - r * r;
            
            auto delta = b * b -  4.0f * a * c;
            auto delta_sqrt = std::sqrt(delta);
              
            if(delta > 0){
                t = std::min( (-b - delta_sqrt)/ (2* a) , (-b + delta_sqrt) / (2*a) );
                glm::vec3 pi = ray.o+ t * ray.d;
                normal = glm::normalize(pi - cen);
                return t > 0;
            }

            return false;
        }
};

class Plane: public Object{
    private:

    public:
        glm::vec3 N_;
        float D_;

        
        Plane(Point color, Point N, float d, float kd = 0.9, float ks = 0.5, float n  = 5.f, float idr = 0): 
            Object(color, kd, ks, n, idr), N_(N), D_(d) {

        }

        bool intersect(const Ray& ray, float& t, glm::vec3 &normal) override{


            t = -(glm::dot(N_, ray.o) + D_) / ( glm::dot(N_, ray.d) );
            
            if(t > 0){
                normal = N_;
                return true;
            }

            return false;
        }
};

#endif