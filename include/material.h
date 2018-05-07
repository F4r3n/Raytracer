#ifndef MATERIAL
#define MATERIAL
#include "Ray.hpp"
#include "hitable.hpp"
#include "Math/helpers.h"
#include <iostream>
#include "texture.h"

typedef fm::math::vec3 vec3;




class Material
{
public:
    virtual bool Scatter(const Ray &ray, const HitRecord &record, fm::math::vec3 &attenuation, Ray & scattered) = 0;
};

class Lambertian : public Material
{
public:
    Lambertian(const fm::math::vec3 albedo): albedo(albedo)
    {
    }
    virtual bool Scatter(const Ray &ray, const HitRecord &record, fm::math::vec3 &attenuation, Ray &scattered)
    {
        fm::math::vec3 target = record.p + record.normal + fm::math::randomInUnitSphere();
        scattered = Ray(record.p, target - record.p);
        attenuation = albedo;
        if(texture && texture->IsValid())
        {
            fm::math::vec4 colorTexture = texture->GetValue(record.uv.x, record.uv.y);
            attenuation.x*=colorTexture.x;
            attenuation.y*=colorTexture.y;
            attenuation.z*=colorTexture.z;
        }

        return true;
    }
    fm::math::vec3 albedo;
    std::shared_ptr<Texture> texture = nullptr;
};

class Metal : public Material
{
public:
    Metal(const fm::math::vec3 albedo, float f = 0.1f): albedo(albedo)
    {
        fFuzz = fm::math::clamp(f, 1.0f, 0.0f);
    }
    virtual bool Scatter(const Ray &ray, const HitRecord &record, fm::math::vec3 &attenuation, Ray &scattered)
    {
        fm::math::vec3 reflect = reflectance(normalize(ray.GetDirection()), record.normal);
        attenuation = albedo;
        scattered = Ray(record.p, reflect + fFuzz*fm::math::randomInUnitSphere()*dot(reflect, record.normal));
        return dot(scattered.GetDirection(), record.normal) > 0;
    }
    fm::math::vec3 albedo;
    float fFuzz;
};

class Dielectric : public Material
{
public:
    Dielectric(float ri) : refIDx(ri) {}
    virtual bool Scatter(const Ray &ray, const HitRecord &record, fm::math::vec3 &attenuation, Ray &scattered)
    {
        fm::math::vec3 outwardNormal;
        vec3 reflected = reflectance(ray.GetDirection(), record.normal);
        float niOverNt;
        attenuation = vec3(1.0,1.0,1.0);
        vec3 refracted;
        float cosine;
        float reflectProbe;
        if(dot(ray.GetDirection(), record.normal) > 0)
        {
            outwardNormal = -record.normal;
            niOverNt = refIDx;
            cosine = refIDx*dot(ray.GetDirection(), record.normal)/(length(ray.GetDirection()));
        }else
        {
            outwardNormal = record.normal;
            niOverNt = 1.0f/refIDx;
            cosine = -dot(ray.GetDirection(), record.normal)/(length(ray.GetDirection()));
        }

        if(refract(ray.GetDirection(), outwardNormal, niOverNt, refracted))
        {
            reflectProbe = fm::math::schlick(cosine, refIDx);
        }else {
            reflectProbe = 1.0f;
        }

        if(fm::math::floatRand() < reflectProbe)
        {
            scattered = Ray(record.p, reflected);
        }else
        {
            scattered = Ray(record.p, refracted);
        }

        return true;
    }

     float refIDx;
};


#endif // MATERIAL

