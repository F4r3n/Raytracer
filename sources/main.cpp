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
#include <chrono>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <iomanip>
#define WITH_ENKITS 0

class Profile {

public:
    Profile() {
        start = std::chrono::system_clock::now();
        running= true;
    }
    Profile(const std::string& name)
    {
        fName = name;
        start = std::chrono::system_clock::now();
        running = true;
    }

    ~Profile()
    {
        stop();
    }

    void stop()
    {
        end = std::chrono::system_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        running = false;
    }
    unsigned long elapsed = 0;

private:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
    std::string fName;
    bool running = false;
};

using namespace fm::math;



bool run = true;

struct JobData
{
    unsigned long width;
    unsigned long height;
    unsigned char* backBuffer;
    Camera *camera;
    World *world;
    unsigned long *rayCount;
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


vec3 Color(const Ray &r, World *world, int depth, unsigned long &rayCount)
{
    rayCount++;
    HitRecord record;
    if(world->Hit(r, 0.001f, std::numeric_limits<float>::max(), record))
    {
        Ray scattered;
        vec3 attenuation;
        if(depth < 50 && record.material->Scatter(r, record, attenuation, scattered))
        {
            return attenuation*Color(scattered, world, depth + 1, rayCount);
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
                 float u = float(x + fm::math::floatRand())/float(data.width);
                 float v = float(y + fm::math::floatRand())/float(data.height);
                 Ray r = data.camera->GetRay(u,v);
                 vec3 col = Color(r, data.world, 0,*data.rayCount);
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

static void UpdateRayTracer(unsigned long width, unsigned long height, unsigned char* buffer, Camera *camera, World *world, unsigned long &rayCount)
{
   const unsigned long int ns = 64;
unsigned long y;
unsigned long x;
const float invWidth = 1.0f/float(width);
const float invHeight = 1.0f/float(height);

   #pragma omp parallel for \
    shared(camera, world, buffer, rayCount,  width, height) \
    private(y, x) schedule(dynamic)
    for(y = 0; y < height; ++y)
    {
        unsigned char* backBuffer = buffer + width*3*(height - y - 1);

         for(x = 0; x < width; ++x)
         {
             vec3 color(0,0,0);
             for(unsigned long s = 0; s < ns; ++s)
             {
                 float u = float(x + fm::math::floatRand())*invWidth;
                 float v = float(y + fm::math::floatRand())*invHeight;
                 Ray r = camera->GetRay(u,v);
                 vec3 col = Color(r, world, 0, rayCount);
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


unsigned long Draw(unsigned char* backBuffer, unsigned long width, unsigned long height, World *world)
{
    vec3 lookfrom(0,0.3,0.5);
    vec3 lookat(0,0.4,1);
    Camera cam(lookfrom,lookat,vec3(0,1,0),60,float(width)/float(height), 0.01f, length(lookfrom - lookat));
    unsigned long rayCount = 0;
#if WITH_ENKITS
    std::cout << "enkits" << std::endl;
    JobData args;
    args.width = width;
    args.height = height;
    args.backBuffer = backBuffer;
    args.camera = &cam;
    args.world = world;
    args.rayCount = &rayCount;
    enkiTaskSet* task = enkiCreateTaskSet(g_TS, UpdateRayTracer);
    bool threaded = true;
    enkiAddTaskSetToPipeMinRange(g_TS, task, &args, height, threaded ? 4 : height);

    enkiWaitForTaskSet(g_TS, task);
    enkiDeleteTaskSet(task);
#else
    std::cout << "OpenMP" << std::endl;
    UpdateRayTracer(width, height, backBuffer,&cam, world, rayCount);
#endif
return rayCount;

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
    std::unique_ptr<Hitable> sphere4 = std::make_unique<Sphere>(vec3(-1,0,-1),0.5f, std::make_shared<Dielectric>(1.5f));
    std::unique_ptr<Hitable> sphere5 = std::make_unique<Sphere>(vec3(-1,0,-1),-0.45f, std::make_shared<Dielectric>(1.5f));


    world.AddObject(sphere);
    world.AddObject(sphere2);
    world.AddObject(sphere3);
    world.AddObject(sphere4);
    world.AddObject(sphere5);


    if(TTF_Init() == -1)
    {
        fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError());
          exit(EXIT_FAILURE);
    }
    TTF_Font* arial = TTF_OpenFont("Resources/arial.ttf", 12);

    while (run)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) run = false;
        }
        Profile p;
        unsigned long rayCount = Draw(data, width, height, &world);
        p.stop();
        std::cout << rayCount << " " << p.elapsed/1000.0f << std::endl;
        std::stringstream stream;
        stream << std::fixed << std::setprecision(2) << (rayCount/(p.elapsed/(1000.0f)))/1000000.0f;
        std::string s = stream.str() + " mr/s";
        SDL_Surface* surfaceMessage = TTF_RenderText_Blended(arial, s.c_str(), {255, 0, 0});

        SDL_BlitSurface(surfaceMessage, NULL, imageSurface, NULL);
        SDL_BlitSurface(imageSurface, NULL, screen, &surfaceToBlit);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(33);
        SDL_FreeSurface(surfaceMessage);

    }
    SDL_FreeSurface(imageSurface);

    SDL_DestroyWindow(window);
    SDL_Quit();


    ShutdownTest();
    delete data;
    return 0;
}
