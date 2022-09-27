#ifndef _CAMERA_H_
#define _CAMERA_H_


#include <glm/glm.hpp>
#include <math.h>
#include <vector>
#include "Object.h"
#include "light.h"


using Point  = glm::vec3;



class Camera{

    private:

    float f,w, h, a,b;
    Point pos; // position of camera   
    Point xe,ye,ze; // unitary vectors that define the camera

    friend class World;

    void fresnel(glm::vec3 I, glm::vec3 N, float& ior, float &kr){

        float cosi = glm::clamp(-1.0F, 1.0F, glm::dot(I, N));
        float etai = 1, etat = ior;
        if(cosi > 0)
            std::swap(etai, etat);
        float sint = etai / etat * std::sqrt(std::max(0.0F, 1 - cosi * cosi));
        if(sint >= 1)
            kr = 1;
        else{
            float cost = std::sqrt(std::max(0.0F, 1 - sint * sint));
            cosi = std::abs(cosi);
            float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            kr = (Rs * Rs + Rp * Rp) / 2;
        }
    }

    glm::vec3 refract(glm::vec3  I, glm::vec3 N, float ior){
        float cosi = glm::clamp(-1.0F, 1.0F, glm::dot(I, N));
        float etai = 1, etat = ior;
        auto n = N;
        if(cosi < 0)
            cosi = -cosi;
        else{
            std::swap(etai, etat);
            n = -1.0F*N;
        }
        float eta = etai / etat;
        float k = 1 - eta * eta * (1 - cosi * cosi);
        return k < 0? glm::vec3(0,0,0) : eta * I + (eta * cosi - std::sqrt(k)) * n;
    }

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

        glm::vec3 calculateColor(const Ray &ray, std::vector<Object *> objects, std::vector<Light *> lights, glm::vec3 ambientLight, int prof){
            
            glm::vec3 result(173 / 255.0f,216/ 255.0f,230/ 255.0f);
            glm::vec3 normal;
            
            
            float t = INFINITY;
            Object* min_obj = nullptr;

            if(prof > 7){
                return result;
            }

            for(auto obj: objects){
                float temp_t = INFINITY;
                glm::vec3 t_normal;
                
                if(obj->intersect(ray, temp_t, t_normal)){
                    if(t > temp_t){
                        t = temp_t;
                        min_obj = obj;
                        normal = t_normal;
                    }
                }
            }
                
            if(min_obj){

                glm::vec3  difusa(0,0,0), especular(0,0,0);
                glm::vec3  ambient = ambientLight * min_obj->getKd();
                
                glm::vec3  color_reflexivo(0,0,0);
                glm::vec3  color_refractivo(0,0,0);

                float kr = min_obj->getKs();
                float kt = 0;

                auto pi = ray.o + t * ray.d;
                auto v = glm::normalize(-1.0F * ray.d);   

                for(auto light: lights){
                                            
                    auto pi = ray.o + t * ray.d;
                    auto L = glm::normalize( light->pos - pi );

                    Ray shadowRay{pi+ normal * 0.01f, L};
                    bool sombra = false;

                    for(auto obj: objects){
                        float t_tmp;
                        glm::vec3 tmp;

                        if(obj->intersect(shadowRay,t_tmp, tmp ) ){
                            sombra = true;
                            break;
                        }
                    } 

                    if(!sombra){
                        //difuse component
                        float fd = glm::dot(normal, L);  
                        if(fd > 0){
                            difusa = light->color * min_obj->getKd() * fd;   
                        }
                        
                        auto r = glm::normalize( 2.0f * normal * glm::dot(L, normal)  - L) ;
                        auto v = -glm::normalize(ray.d);
                        float fs = dot(r, v);  

                        if(fs > 0){
                            especular = light->color * min_obj->getKs() * pow(fs , min_obj->getn());   
                        }
                    }

                    bool outside = glm::dot(ray.d, normal) < 0;
                    auto bias = 0.0001f * normal;

                    if(min_obj->getIdr() > 0){
                        fresnel(ray.d, normal, min_obj->idr, kr);
                        if(kr < 1){
                            kt = 1 - kr;
                            Ray refractive_ray;
                            refractive_ray.o = outside? pi - bias :pi + bias;
                            refractive_ray.d = glm::normalize( refract(ray.d, normal, min_obj->idr) );
                            color_refractivo = calculateColor(refractive_ray, objects, lights, ambientLight ,prof+1);
                        }
                    }
                    if(kr > 0){
                        Ray reflexive_ray;
                        reflexive_ray.o  = outside? pi - bias : pi + bias;
                        reflexive_ray.d = glm::normalize(2 * glm::dot(v, normal) * normal - v);
                        color_reflexivo = calculateColor(reflexive_ray, objects, lights, ambientLight, prof + 1);
                    }

                    result = min_obj->get_color() * (ambient +  especular + difusa);
                    result = result + color_reflexivo * kr + color_refractivo * kt;
                    result  = glm::clamp(result, 0.0f,1.0f);
                }
            }
            return result;
        }
};



#endif