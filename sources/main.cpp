#if __unix__
#define SIMD 1
#else
#define SIMD 0
#endif

#include <iostream>
#include "Math/Functions.h"
#include <SDL.h>
#include <mutex>
#include "Ray.hpp"
#include "camera.h"
#include "world.h"
#include <limits>
#include "sphere.h"
#include <random>
#include "Math/helpers.h"
#include "material.h"
#include <omp.h>
#include "profile.h"
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <iomanip>
#include "texture.h"
#define DISPLAY 1
#define EARLY_DISPLAY 1 && DISPLAY

#if EARLY_DISPLAY
#include <thread>
#endif

using namespace fm::math;
bool run = true;


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


static void UpdateRayTracer(unsigned long width, unsigned long height, unsigned char* buffer, Camera *camera, World *world, int ns, unsigned long &rayCount)
{
   unsigned long y;
   unsigned long x;
   const float invWidth = 1.0f/float(width);
   const float invHeight = 1.0f/float(height);

   #pragma omp parallel for \
    shared(camera, world, buffer, rayCount,  width, height, ns) \
    private(y, x) schedule(dynamic)
    for(y = 0; y < height; ++y)
    {
        unsigned char* backBuffer = buffer + width*3*(height - y - 1);

         for(x = 0; x < width; ++x)
         {
             vec3 color(0,0,0);
             for(int s = 0; s < ns; ++s)
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


unsigned long Draw(unsigned char* backBuffer, unsigned long width, unsigned long height, World *world,int ns, vec3 pos = vec3(0,0,0))
{
    vec3 lookfrom = pos;
    vec3 lookat(0,0,1);
    Camera cam(lookfrom,lookat,vec3(0,1,0),90,float(width)/float(height), 0.0f, 1.0f);
    unsigned long rayCount = 0;

    UpdateRayTracer(width, height, backBuffer,&cam, world, ns, rayCount);

    return rayCount;
}

void CopyAvergareRecursif(unsigned int n, unsigned long width, unsigned long height, unsigned char* backBuffer, unsigned char* buffer)
{
    unsigned char* bufferTemp = buffer;
    unsigned char* backBufferTemp = backBuffer;

    for(unsigned long y = 0; y < height; ++y)
    {
        for(unsigned long x = 0; x < width; ++x)
        {
            *backBufferTemp = float(*backBufferTemp*n + *bufferTemp++)/float(n+ 1);
            backBufferTemp++;
            *backBufferTemp = float(*backBufferTemp*n + *bufferTemp++)/float(n+ 1);
            backBufferTemp++;
            *backBufferTemp = float(*backBufferTemp*n + *bufferTemp++)/float(n+ 1);
            backBufferTemp++;
        }
    }
}
static bool over = false;
static unsigned long rayCount;
static unsigned long elapsedTime;
#if EARLY_DISPLAY


void runGraphicThread(unsigned char* backBuffer, unsigned long width, unsigned long height, World *world)
{
    int n = 0;
    unsigned char *data = new unsigned char[width * height * 3];

    while(run)
    {
        if(n < 128)
        {
             Profile p;
            rayCount = Draw(data, width, height, world, 1);
            p.stop();
            elapsedTime = p.elapsed;
            CopyAvergareRecursif(n, width, height, backBuffer, data);
            n++;
        }else {
            over = true;

        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    delete data;
}
#endif


int main()
{
    const unsigned long width = 1280;
    const unsigned long height = 720;
    Texture mainTexture(width, height, 3);
#if DISPLAY
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("Raytracer",
                                           SDL_WINDOWPOS_CENTERED,
                                           SDL_WINDOWPOS_CENTERED,
                                           width, height, 0);

    SDL_ShowWindow(window);
    SDL_Surface * screen = SDL_GetWindowSurface(window);



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
                mainTexture.GetPtr(), width, height, 24, width * 3,
                rmask, gmask, bmask, amask);

    SDL_Rect surfaceToBlit = { 0, 0, width, height };

    if(TTF_Init() == -1)
    {
        fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError());
          exit(EXIT_FAILURE);
    }
    TTF_Font* arial = TTF_OpenFont("Resources/arial.ttf", 12);


#endif


    World world;
    std::shared_ptr<Lambertian> l = std::make_shared<Lambertian>(vec3(1.0,1.0,1.0));
    l->texture = std::make_shared<Texture>("Resources/ground.jpg");
    std::unique_ptr<Hitable> sphere = std::make_unique<Sphere>(vec3(0,0,-1),0.5f, l);
    std::unique_ptr<Hitable> sphere2 = std::make_unique<Sphere>(vec3(0,-100.5f,-1),100.0f, std::make_shared<Lambertian>(vec3(0.8,0.8,0.0)));
    std::unique_ptr<Hitable> sphere3 = std::make_unique<Sphere>(vec3(1,0,-1),0.5f, std::make_shared<Metal>(vec3(0.8,0.6,0.2),0.3f));
    std::unique_ptr<Hitable> sphere4 = std::make_unique<Sphere>(vec3(-1,0,-1),0.5f, std::make_shared<Metal>(vec3(0.8,0.3,0.9),0.9f));
    //std::unique_ptr<Hitable> sphere5 = std::make_unique<Sphere>(vec3(-1,0,-1),-0.45f, std::make_shared<Dielectric>(1.5f));
    //std::unique_ptr<Hitable> sphere6 = std::make_unique<Sphere>(vec3(-1,0,-1.5),0.5f, std::make_shared<Lambertian>(vec3(0.8,0.3,0.3)));


    world.AddObject(sphere);
    world.AddObject(sphere2);
    world.AddObject(sphere3);
    world.AddObject(sphere4);
    //world.AddObject(sphere5);
    //world.AddObject(sphere6);

#if EARLY_DISPLAY
    std::thread t(runGraphicThread, mainTexture.GetPtr(), width, height, &world);
#endif

    bool saved = false;
    while (run)
    {
#if DISPLAY
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) run = false;

        }
#endif

         std::stringstream stream;
#if DISPLAY
#if !EARLY_DISPLAY
        Profile p;
        rayCount = Draw(mainTexture.GetPtr(), width, height, &world);
        p.stop();
        std::cout << rayCount << " " << p.elapsed/1000.0f << std::endl;
        stream << std::fixed << std::setprecision(2) << (rayCount/(p.elapsed/(1000.0f)))/1000000.0f;
#else
         stream << std::fixed << std::setprecision(2) << (rayCount/(elapsedTime/(1000.0f)))/1000000.0f;
#endif

        std::string s = stream.str() + " mr/s";
        SDL_Surface* surfaceMessage = TTF_RenderText_Blended(arial, s.c_str(), {255, 0, 0});


#if !EARLY_DISPLAY
        SDL_BlitSurface(surfaceMessage, NULL, imageSurface, NULL);
#else
        std::cout << s << std::endl;
#endif

        SDL_BlitSurface(imageSurface, NULL, screen, &surfaceToBlit);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(16);
        SDL_FreeSurface(surfaceMessage);

#else
         Profile p;
         rayCount = Draw(mainTexture.GetPtr(), width, height, &world, 128);
         p.stop();
         std::cout << rayCount << " " << p.elapsed/1000.0f << std::endl;
         stream << std::fixed << std::setprecision(2) << (rayCount/(p.elapsed/(1000.0f)))/1000000.0f;
         std::string s = stream.str() + " mr/s";
         std::cout << s << std::endl;
         over = true;
         if(over && !saved)
         {
             run = false;
         }

#endif
        if(over && !saved)
        {

            mainTexture.SaveDataToFile("final.png");

        }
    }

#if EARLY_DISPLAY
    t.join();
#endif
    #if DISPLAY
    SDL_FreeSurface(imageSurface);

    SDL_DestroyWindow(window);
    SDL_Quit();
#endif

    return 0;
}
