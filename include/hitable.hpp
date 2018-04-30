#ifndef HITABLE_HPP
#define HITABLE_HPP

#include "Ray.hpp"

struct HitRecord
{
    float t;
    fm::math::vec3 p;
    fm::math::vec3 normal;
};

class Hitable
{
public:
    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const = 0;
};

#endif // HITABLE_HPP

