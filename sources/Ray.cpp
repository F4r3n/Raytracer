#include "Ray.hpp"

Ray::Ray(const fm::math::vec3 &inOrigin, const fm::math::vec3 inDirection)
{
    fOrigin = inOrigin;
    fDirection = inDirection;
}

fm::math::vec3 Ray::PointAt(float t) const
{
   return fOrigin + t*fDirection;
}


Ray::~Ray()
{

}
