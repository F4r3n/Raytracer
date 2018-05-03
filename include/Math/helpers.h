#pragma once
#include "Math/Vector3.h"
#include <random>
namespace fm {
namespace math {

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


