#ifndef MATERIAL
#define MATERIAL
#include "Ray.hpp"
#include "hitable.hpp"
#include "Math/helpers.h"
#include <iostream>

template <typename T>
T clamp(T v, T max, T min)
{
    if(v < min) return min;
    if(v > max) return max;
    return v;
}

class Material
{
public:
    virtual bool Scatter(const Ray &ray, const HitRecord &record, fm::math::vec3 &attenuation, Ray & scattered) = 0;
};

class Lambertian : public Material
{
public:
    Lambertian(const fm::math::vec3 albedo): albedo(albedo){}
    virtual bool Scatter(const Ray &ray, const HitRecord &record, fm::math::vec3 &attenuation, Ray &scattered)
    {
        fm::math::vec3 target = record.p + record.normal + fm::math::randomInUnitSphere();
        scattered = Ray(record.p, target - record.p);
        attenuation = albedo;
        //std::cout << albedo << std::endl;
        return true;
    }
    fm::math::vec3 albedo;
};

class Metal : public Material
{
public:
    Metal(const fm::math::vec3 albedo, float f = 0.1f): albedo(albedo)
    {
        fFuzz = clamp(f, 1.0f, 0.0f);
    }
    virtual bool Scatter(const Ray &ray, const HitRecord &record, fm::math::vec3 &attenuation, Ray &scattered)
    {
        fm::math::vec3 reflect = reflectance(normalize(ray.GetDirection()), record.normal);
        attenuation = albedo;
        scattered = Ray(record.p, reflect + fFuzz*fm::math::randomInUnitSphere());
        return dot(scattered.GetDirection(), record.normal) > 0;
    }
    fm::math::vec3 albedo;
    float fFuzz;
};

#endif // MATERIAL

