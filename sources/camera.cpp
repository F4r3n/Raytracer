#include "camera.h"
#include "Math/Functions.h"
#include <iostream>

using namespace fm::math;
//TODO add focus camera
Camera::Camera(const fm::math::vec3 &origin, const fm::math::vec3 &direction, const fm::math::vec3 &vup, float fov, float aspect)
{
    fOrigin = origin;
    fDirection = direction;

    float theta = fov*fm::math::pi()/180.0f;
    float halfHeight = fm::math::tan(theta/2.0f);
    float halfWidth = halfHeight*aspect;

    w = normalize(direction - origin);
    u = normalize(cross(vup, w));
    v = cross(w, u);

    lowerLeftCorner = origin - halfWidth*u - halfHeight*v - w;

    horizontal = 2*halfWidth*u;
    vertical = 2*halfHeight*v;

}

Ray Camera::GetRay(float x, float y) const
{
    return Ray(fOrigin, normalize(lowerLeftCorner + x*horizontal + y*vertical - fOrigin));
}



