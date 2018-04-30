#pragma once

#include "Math/Vector3.h"

class Ray
{
public:
Ray(const fm::math::vec3 &inOrigin, const fm::math::vec3 inDirection);
~Ray();
inline const fm::math::vec3& GetOrigin() const { return fOrigin;}
inline const fm::math::vec3& GetDirection() const {return fDirection;}

fm::math::vec3 PointAt(float t) const;

private:
    fm::math::vec3 fOrigin;
    fm::math::vec3 fDirection;

};
