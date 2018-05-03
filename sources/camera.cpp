#include "camera.h"
#include "Math/Functions.h"
#include <iostream>
#include "Math/helpers.h"
using namespace fm::math;
//TODO add focus camera
Camera::Camera(const fm::math::vec3 &origin, const fm::math::vec3 &direction, const fm::math::vec3 &vup, float fov, float aspect, float aperture, float focusDist)
{
    lensRadius = aperture;
    fOrigin = origin;
    fDirection = direction;

    float theta = fov*fm::math::pi()/180.0f;
    float halfHeight = fm::math::tan(theta/2.0f);
    float halfWidth = halfHeight*aspect;

    w = normalize(direction - origin);
    u = normalize(cross(vup, w));
    v = cross(w, u);

    //lowerLeftCorner = vec3(-halfWidth, -halfHeight, -1.0f);
    lowerLeftCorner = origin - halfWidth*focusDist*u - halfHeight*v*focusDist - w;

    horizontal = 2*halfWidth*focusDist*u;
    vertical = 2*halfHeight*v*focusDist;

}

Ray Camera::GetRay(float x, float y) const
{
    vec3 rd = lensRadius*fm::math::randomInUnitSphere();
    vec3 offset = u*rd.x + v*rd.y;
    return Ray(fOrigin + offset, normalize(lowerLeftCorner + x*horizontal + y*vertical - fOrigin - offset));
}



