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
                auto ray = camera->raycast(x,y);
                auto color = camera->calculateColor(ray, objects, lights, ambientLight, 1);
                auto& pix = img.at<cv::Vec3b>(y, x);

                // auto scale_col = glm::clamp(color, 0.0f,1.0f);

                pix[0] = color [2] * 255;
                pix[1] = color [1] * 255;
                pix[2] = color [0] * 255;
            }
        }

        cv::imwrite("img/test.jpg", img);

    }

};


#endif