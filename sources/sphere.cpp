#include "sphere.h"
#include "material.h"
using namespace fm::math;
Sphere::Sphere(const fm::math::vec3 center, float radius, const std::shared_ptr<Material> &mat)
{
    fCenter = center;
    fRadius = radius;
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
    float a = 2*dot(r.GetDirection());
    float invertA = 1/a;
    float c = dot(oc) - fRadius*fRadius;
    float b = 2*dot(oc, r.GetDirection());
    float discriminant = (b*b - 2*a*c);

    if(discriminant > 0)
    {
        float temp = (-b - sqrt(discriminant))*invertA;
        if(temp < tMax && temp > tMin)
        {
            record.t = temp;
            record.p = r.PointAt(record.t);
            record.normal = (record.p - fCenter)/fRadius;
            fm::math::GetSphereUV(record.normal, record.uv);

            return true;
        }
        temp = (-b + sqrt(discriminant))*invertA;
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



