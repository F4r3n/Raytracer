#pragma once
#include "Math/Vector3.h"
namespace fm {
namespace math {
/*****RANDOM IN SPHERE UNIT****/
inline vec3 randomInUnitSphere()
{
    vec3 p;
    do
    {
        p = 2.0f*vec3(drand48(), drand48(), drand48()) - vec3(1,1,1);

    }while(sqrt(dot(p))>= 1.0);
    return p;
}
inline vec3 reflectance(const vec3 &v, const vec3 &n)
{
    return v - 2.0f*dot(v,n)*n;
}
}
}


