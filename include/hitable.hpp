#ifndef HITABLE_HPP
#define HITABLE_HPP
#include <memory>
#include "Ray.hpp"
class Material;
struct HitRecord
{
    float t;
    fm::math::vec3 p;
    fm::math::vec3 normal;
    Material* material;
};

class Hitable
{
public:
    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const = 0;
};

#endif // HITABLE_HPP

