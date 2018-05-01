#include <iostream>
#include "Math/Functions.h"
#include <SDL.h>
#include <mutex>
#include "Ray.hpp"
#include "camera.h"
#include "TaskScheduler_c.h"
#include "world.h"
#include <limits>
#include "sphere.h"
#include <random>
#include "Math/helpers.h"
#include "material.h"
#include <omp.h>

using namespace fm::math;



bool run = true;

struct JobData
{
    unsigned long width;
    unsigned long height;
    unsigned char* backBuffer;
    Camera *camera;
    World *world;
    unsigned char* buffer;
};

static enkiTaskScheduler* g_TS;

void InitializeTest()
{
    g_TS = enkiNewTaskScheduler();
    enkiInitTaskScheduler(g_TS);
}

void ShutdownTest()
{
    enkiDeleteTaskScheduler(g_TS);
}


vec3 Color(const Ray &r, World *world, int depth)
{
    HitRecord record;
    if(world->Hit(r, 0.001f, std::numeric_limits<float>::max(), record))
    {
        Ray scattered;
        vec3 attenuation;
        if(depth < 50 && record.material->Scatter(r, record, attenuation, scattered))
        {
            return attenuation*Color(scattered, world, depth + 1);
        }else
        {
            return vec3(0,0,0);
        }
    }

    vec3 dir = normalize(r.GetDirection());
    float t = 0.5f*(dir.y + 1.0);
    return (1.0f - t)*vec3(1.0,1.0,1.0) + t*vec3(0.5f,0.7f,1.0f);
}


static void UpdateRayTracer(uint32_t start, uint32_t end, uint32_t threadnum, void* data_)
{
    JobData& data = *(JobData*)data_;
    unsigned long int ns = 12;
    for(unsigned long y = start; y < end; ++y)
    {
        unsigned char* backBuffer = data.backBuffer + data.width*3*(data.height - y - 1);

         for(unsigned long x = 0; x < data.width; ++x)
         {
             vec3 color(0,0,0);
             for(unsigned long s = 0; s < ns; ++s)
             {
                 float u = float(x + drand48())/float(data.width);
                 float v = float(y + drand48())/float(data.height);
                 Ray r = data.camera->GetRay(u,v);
                 vec3 col = Color(r, data.world, 0);
                 color += col;
             }
             color/=(float)ns;
             color = vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));
             backBuffer[0] = color.x*255.99f;
             backBuffer[1] = color.y*255.99f;
             backBuffer[2] = color.z*255.99f;
             backBuffer += 3;
         }
     }

}

static void UpdateRayTracer(unsigned long width, unsigned long height, unsigned char* buffer, Camera *camera, World *world)
{
   const unsigned long int ns = 12;
unsigned long y;
unsigned long x;
   #pragma omp parallel for \
    shared(camera, world, buffer, width, height) \
    private(y, x) schedule(static)
    for(y = 0; y < height; ++y)
    {
        unsigned char* backBuffer = buffer + width*3*(height - y - 1);

         for(x = 0; x < width; ++x)
         {
             vec3 color(0,0,0);
             for(unsigned long s = 0; s < ns; ++s)
             {
                 float u = float(x + drand48())/float(width);
                 float v = float(y + drand48())/float(height);
                 Ray r = camera->GetRay(u,v);
                 vec3 col = Color(r, world, 0);
                 color += col;
             }
             color/=(float)ns;
             color = vec3(sqrt(color.x), sqrt(color.y), sqrt(color.z));
             backBuffer[0] = color.x*255.99f;
             backBuffer[1] = color.y*255.99f;
             backBuffer[2] = color.z*255.99f;
             backBuffer += 3;
         }
     }

}


void Draw(unsigned char* backBuffer, unsigned long width, unsigned long height, World *world)
{
    Camera cam(vec3(0,0,0),vec3(0,0,1),vec3(0,1,0),90,float(width)/float(height));
/*
    JobData args;
    args.width = width;
    args.height = height;
    args.backBuffer = backBuffer;
    args.camera = &cam;
    args.world = world;
    enkiTaskSet* task = enkiCreateTaskSet(g_TS, UpdateRayTracer);
    bool threaded = true;
    enkiAddTaskSetToPipeMinRange(g_TS, task, &args, height, threaded ? 4 : height);

    enkiWaitForTaskSet(g_TS, task);
    enkiDeleteTaskSet(task);*/
    UpdateRayTracer(width, height, backBuffer,&cam, world);



}

int main()
{

    const unsigned long width = 256;
    const unsigned long height = 128;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("Raytracer",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           width, height, 0);

    SDL_ShowWindow(window);
    SDL_Surface * screen = SDL_GetWindowSurface(window);

    unsigned char *data = new unsigned char[width * height * 3];

    Uint32 rmask, gmask, bmask, amask;


    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      int shift = 8
      rmask = 0xff000000 >> shift;
      gmask = 0x00ff0000 >> shift;
      bmask = 0x0000ff00 >> shift;
      amask = 0x000000ff >> shift;
    #else // little endian, like x86
      rmask = 0x000000ff;
      gmask = 0x0000ff00;
      bmask = 0x00ff0000;
      amask = 0;
    #endif

    SDL_Surface * imageSurface = SDL_CreateRGBSurfaceFrom(
                data, width, height, 24, width * 3,
                rmask, gmask, bmask, amask);

    SDL_Rect surfaceToBlit = { 0, 0, width, height };

    InitializeTest();


    World world;
    std::unique_ptr<Hitable> sphere = std::make_unique<Sphere>(vec3(0,0,-1),0.5f, std::make_shared<Lambertian>(vec3(0.8,0.3,0.3)));
    std::unique_ptr<Hitable> sphere2 = std::make_unique<Sphere>(vec3(0,-100.5f,-1),100.0f, std::make_shared<Lambertian>(vec3(0.8,0.8,0.0)));
    std::unique_ptr<Hitable> sphere3 = std::make_unique<Sphere>(vec3(1,0,-1),0.5f, std::make_shared<Metal>(vec3(0.8,0.6,0.2),0.3f));
    std::unique_ptr<Hitable> sphere4 = std::make_unique<Sphere>(vec3(-1,0,-1),0.5f, std::make_shared<Metal>(vec3(0.8,0.8,0.8),0.8f));


    world.AddObject(sphere);
    world.AddObject(sphere2);
    world.AddObject(sphere3);
    world.AddObject(sphere4);


    while (run)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) run = false;
        }
        Draw(data, width, height, &world);
        SDL_BlitSurface(imageSurface, NULL, screen, &surfaceToBlit);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(33);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();


    ShutdownTest();
    delete data;
    return 0;
}
