#ifndef SPHERE
#define SPHERE
#include "hitable.hpp"
#include <memory>
class Material;
class Sphere : public Hitable
{
public:
    Sphere(const fm::math::vec3 center, float radius, const std::shared_ptr<Material> &mat);
    Sphere();
    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const;

    fm::math::vec3 fCenter;
    float fRadius;
    std::shared_ptr<Material> fMat;
    void SetRadius(float radius)
    {
        fRadius = radius;
        fSquaredRadius = fRadius*fRadius;
    }

private:
    float fSquaredRadius;
};

#endif // SPHERE

