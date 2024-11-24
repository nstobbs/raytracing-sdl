#ifndef RENDER_HEADER
#define RENDER_HEADER

#include <chrono>
#include <vector>
#include <thread>
#include <stdio.h>
#include "vec3_uint8.h"

#include "rt-headers/camera.h"
#include "rt-headers/hittable.h"
#include "rt-headers/hittable_list.h"
#include "rt-headers/material.h"
#include "rt-headers/sphere.h"

using namespace std::chrono_literals;
bool ENABLE_DELAY = false;
std::chrono::milliseconds DELAY = 1ms;

void renderChunk(std::vector<std::vector<color_uint8>> &framebuffer, camera &renderCamera, hittable_list &renderScene,  int width, int height, int slices, int startPos) 
{
    printf("Thread %i starting...\n", startPos);
    int threadID = startPos; 
    startPos = (height / slices) * startPos;
    for (int y = startPos; y < (startPos + (height / slices)); y++)
    {
        for(int x = 0; x < width; x++)
        {
            if(ENABLE_DELAY)
                std::this_thread::sleep_for(DELAY);
            renderCamera.calPixelColor(x, y, renderScene, framebuffer);
        }
    }
    printf("Thread %i Finished...\n", threadID);
}

void render(std::vector<std::vector<color_uint8>> &framebuffer, camera &renderCamera, hittable_list &renderScene, int width, int height, const int threadNum)
{
    std::vector<std::thread> threads;
    for(int t = 0; t < threadNum; t++)
    {
        printf("Creating Thread %i\n", t);
        std::thread thread(renderChunk, std::ref(framebuffer), std::ref(renderCamera), std::ref(renderScene), width, height, threadNum, t);
        threads.push_back(move(thread));

    }  
    for (int ti = 0; ti < threadNum; ti++)
    {
        printf("Detaching Thread %i\n", ti);
        threads[ti].detach();
    }
}

void createBlank2DVectorColor(std::vector<std::vector<color_uint8>> &framebuffer, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        std::vector<color_uint8> row(width);
        framebuffer.push_back(row);

        for (int x = 0; x < width; x++)
        {   
            color_uint8 result;
            row.push_back(result);
        }
    }
}

hittable_list createWorld()
{
    /*
    Builds and creates the scene for the
    camera to render later. 
    */
    hittable_list world;

    auto groundMaterial = make_shared<diffuse>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, groundMaterial));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto chooseMat = randomDouble();
            point3 center(a + 0.9*randomDouble(), 0.2, b + 0.9*randomDouble());
            if ((center - point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<material> sphereMaterial;

                if (chooseMat < 0.8)
                {
                    auto albedo = color::random() * color::random();
                    sphereMaterial = make_shared<diffuse>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
                } else if (chooseMat < 0.95){
                    auto albedo = color::random();
                    auto fuzz = randomDouble(0, 0.5);
                    sphereMaterial = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
                } else {
                    sphereMaterial = make_shared<glass>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
                }
            }
        }
    }

    auto material1 = make_shared<glass>(1.5);
    world.add(make_shared<sphere>(point3(0,1,0), 1.0, material1));

    auto material2 = make_shared<diffuse>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4,1,0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4,1,0), 1.0, material3));
    return world;
}
#endif