#include "sphere.h"
#include "material.h"
#include "Math/constants.hpp"
using namespace fm::math;
Sphere::Sphere(const fm::math::vec3 center, float radius, const std::shared_ptr<Material> &mat)
{
    fCenter = center;
    fRadius = radius;
    fSquaredRadius = fRadius*fRadius;
    fMat = mat;
}

Sphere::Sphere()
{
    fCenter = vec3(0,0,0);
    fRadius = 0.f;
}

bool Sphere::Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const
{
    record.material = fMat.get();
    vec3 oc = r.GetOrigin() - fCenter;

    float c = dot(oc) - fSquaredRadius;
    float b = dot(oc, r.GetDirection());
    float discriminant = (b*b - c);

    if(discriminant > 0)
    {
        float temp = (-b - sqrt(discriminant));
        if(temp < tMax && temp > tMin)
        {
            record.t = temp;
            record.p = r.PointAt(record.t);
            record.normal = (record.p - fCenter)/fRadius;
            fm::math::GetSphereUV(record.normal, record.uv);

            return true;
        }
        temp = (-b + sqrt(discriminant));
        if(temp < tMax && temp > tMin)
        {
            record.t = temp;
            record.p = r.PointAt(record.t);
            record.normal = (record.p - fCenter)/fRadius;
            fm::math::GetSphereUV(record.normal, record.uv);
            return true;
        }
    }
    return false;
}



