#pragma once
#include "Math/Vector3.h"
#include "Math/Functions.h"
#include "Math/Vector2.h"
#include <random>
#include <cmath>
namespace fm {
namespace math {

template <typename T>
T clamp(T v, T max, T min)
{
    if(v < min) return min;
    if(v > max) return max;
    return v;
}

inline void GetSphereUV(const vec3 &p, vec2 &uv)
{
    float phi = std::atan2(p.z, p.x);
    float theta = std::asin(p.y);
    uv.x = 1 - (phi + pi())/(twoPi());
    uv.y = (theta + halfPi())/(pi());
}

inline float floatRand() {
    static thread_local std::mt19937 generator;
    std::uniform_real_distribution<float> distribution(0.0f,1.0f);
    return distribution(generator);
}

/*****RANDOM IN SPHERE UNIT****/
inline vec3 randomInUnitSphere()
{
    vec3 p;
    do
    {
        p = 2.0f*vec3(floatRand(), floatRand(), floatRand()) - vec3(1,1,1);

    }while(sqrt(dot(p))>= 1.0);
    return p;
}
inline vec3 reflectance(const vec3 &v, const vec3 &n)
{
    return v - 2.0f*dot(v,n)*n;
}
inline bool refract(const vec3& v, const vec3 &n, float niOverNt, vec3 &refracted)
{
    vec3 uv = normalize(v);
    float dt = dot(uv, n);
    float discriminant = 1.0f -niOverNt*niOverNt*(1 - dt*dt);
    if(discriminant > 0)
    {
        refracted = niOverNt*(uv-n*dt) - n*(float(sqrt(discriminant)));
        return true;
    }
    return false;
}

inline float schlick(float cosine, float refIDx)
{
    float r0 = (1.0f - refIDx)/( 1.0f + refIDx);
    r0 = r0*r0;
    return r0 + (1 - r0)*pow((1-cosine),5);
}

}

}


