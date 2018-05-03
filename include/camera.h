#ifndef CAMERA_H
#define CAMERA_H
#include "Math/Vector3.h"
#include "Ray.hpp"
class Camera
{
public:

    Camera(const fm::math::vec3 &origin, const fm::math::vec3 &direction, const fm::math::vec3 &vup, float fov, float aspect, float aperture = 1.0f, float focusDist = 1.0f);
    Ray GetRay(float u, float v) const;
private:
    fm::math::vec3 horizontal;
    fm::math::vec3 vertical;
    fm::math::vec3 v,u,w;
    fm::math::vec3 lowerLeftCorner;
    fm::math::vec3 fOrigin;
    fm::math::vec3 fDirection;
    float lensRadius;
};

#endif // CAMERA_H
