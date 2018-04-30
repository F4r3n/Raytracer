#ifndef SPHERE
#define SPHERE
#include "hitable.hpp"
class Sphere : public Hitable
{
public:
    Sphere(const fm::math::vec3 center, float radius);
    Sphere();
    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const;

    fm::math::vec3 fCenter;
    float fRadius;

};

#endif // SPHERE

