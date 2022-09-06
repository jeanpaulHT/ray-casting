#ifndef _WORLD_H
#define _WORLD_H

#include "Camera.h"
#include "Object.h"
#include "light.h"

#include "aux/renderlib.h"

#include <vector>
#include <bitset>
#include <cmath>
#include <string>


#define DIM  256

const int INF = 2e6;


class World{
    
    // Light ambient; 
    private:
    Camera* camera;
    std::vector<Object*> objects;
    std::vector<Light*> lights;
    glm::vec3 ambientLight = {0.1f,0.1f,0.1f};

    void fresnel(glm::vec3 I, glm::vec3 N, float& ior, float &kr){
        float cosi = glm::clamp(-1.0f, 1.0f, glm::dot(I, N));
        float etai = 1; float etat = ior;

        if(cosi > 0){ std::swap(etai, etat);}
        float sint = etai/etat * std::sqrt(std::max(0.0f, 1 - cosi * cosi));

        if(sint >= 1){
            kr = 1;
        }
        else{
            float cost = std::sqrt(std::max(0.0f, 1 - sint * sint));
            cosi = std::abs(cosi);
            float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));   
            float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            float kr = (Rs * Rs + Rp * Rp) / 2;
        }
    }

    public:

    World(Camera* cam) {
        camera = cam;
    }

    World(Camera* cam, std::vector<Object*> vo, std::vector<Light*> vl): objects(vo), lights(vl) {
        camera = cam;
    }

    void insertObj(Object* obj){
        objects.push_back(obj);
    }

    void insertLight(Light* lgt){
        lights.push_back(lgt);
    }
    

    void render(){

        cv::Mat img(camera->h, camera->w,CV_8UC3,cv::Scalar(0,0,0));


        for(int x = 0; x < camera->w; x++){
            for(int y = 0; y < camera->h; y++){
                
                //BGR is default order for opencv


                glm::vec3 color(173 / 255.0f,216/ 255.0f,230/ 255.0f);
                glm::vec3 normal;

                auto ray = camera->raycast(x,y);
                auto& pix = img.at<cv::Vec3b>(y, x);

                float min_t = INFINITY;
                Object* min_obj = nullptr;

                for(auto obj: objects){
                    float t = INFINITY;
                    glm::vec3 t_normal;

                    if(obj->intersect(ray, t, t_normal)){
                        if(min_t > t){
                            min_t = t;
                            min_obj = obj;
                            normal = t_normal;
                        }
                    }
                }
                
                // if intersection
                if(min_obj){

                    Ray shadowRay{ray.o};

                    float kr = min_obj->getKs();
                    float kt = 0;

                    for(auto light: lights){
                        glm::vec3  ambient = ambientLight * min_obj->getKd();
                        
                        auto pi = ray.o + min_t * ray.d;
                        auto L = glm::normalize( light->pos - pi );

                        Ray shadowRay{pi, L};
                        float t_tmp;
                        glm::vec3 tmp;
                        bool sombra = false;

                        for(auto obj: objects){
                            if(obj != min_obj && obj->intersect(shadowRay,t_tmp, tmp ) ){
                                sombra = true;
                                break;
                            }
                        } 
                        if(sombra){
                            color = min_obj->get_color() * ambient;
                            break;
                        }

                        float fd = glm::dot(normal, L);  
                        glm::vec3 difuso(0,0,0);
                        if(fd > 0){
                            difuso = light->color * min_obj->getKd() * fd;   
                        }
                        auto r = ( 2.0f * normal * glm::dot(normal, L)  - L) ;
                        auto v = -glm::normalize(ray.d);
                        float fs = dot(r, v);  

                        glm::vec3 especular(0,0,0);                                
                        if(fs > 0){
                            especular = light->color * min_obj->getKs() * pow(fs , min_obj->getn());   
                        }

                        bool outside = glm::dot(ray.d, normal) < 0;
                        auto bias = 0.0001f * normal;

                        if(min_obj->getIdr() > 0){
                            fresnel(ray.d, normal, min_obj->idr, kr);
                            if(kr < 1){
                                kt = 1 - kr;
                                
                            }

                        }


                        color = min_obj->get_color() * (ambient + difuso + especular);
                    }
                }

                auto scale_col = glm::clamp(color, 0.0f,1.0f);

                pix[0] = scale_col [2] * 255;
                pix[1] = scale_col [1] * 255;
                pix[2] = scale_col [0] * 255;
            }
        }

        cv::imwrite("img/test.jpg", img);

    }

};


#endif