#include "plane.hpp"
#include <iostream>
#include "Math/constants.hpp"
using namespace fm::math;
DiskPlane::DiskPlane(fm::math::vec3 origin, fm::math::vec3 normal, float radius, const std::shared_ptr<Material> &mat)
{
    fRadius = radius;
    fNormal = normal;
    fOrigin = origin;
    fMat = mat;
}

bool DiskPlane::Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const
{
    float den = dot(r.GetDirection(),fNormal);
    if(den > EPSILON)
    {
        float t = dot((fOrigin - r.GetOrigin()), fNormal)/den;
        if(t > 0 && t < tMax && t > tMin)
        {
            vec3 temp = r.PointAt(t);
            if(length(fOrigin - temp) < fRadius)
            {
               record.p = temp;
               record.normal = fNormal;
               record.material = fMat.get();
               return true;
            }
        }
    }

    return false;
}



RectPlane::RectPlane(fm::math::vec3 origin, fm::math::vec3 normal, const vec2 size, const std::shared_ptr<Material> &mat)
{
    fSize = size*0.5f;
    fNormal = normal;
    fOrigin = origin;
    fMat = mat;


    u = normalize(cross(vec3(0,1,0)*length(fNormal), fNormal));
    v = normalize(cross(fNormal, u));
}

bool RectPlane::Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const
{
    float den = dot(r.GetDirection(),fNormal);

    if(den > EPSILON)
    {
        float t = dot(fOrigin - r.GetOrigin(), fNormal)/den;

        if(t > EPSILON && t < tMax && t > tMin)
        {
            vec3 temp = r.PointAt(t);
            vec3 dist = temp - fOrigin;

            if(std::abs(dot(u, dist)) < fSize.x && std::abs(dot(v, dist)) < fSize.y)
            {
               record.t = t;
               record.p = temp;
               record.normal = fNormal;
               record.material = fMat.get();
               record.uv = vec2(std::abs(dist.x/fSize.x), std::abs(dist.y/fSize.y));
               return true;
            }
        }
    }
    return false;
}


XYRect::XYRect()
{

}

XYRect::XYRect(const fm::math::vec4 &pos, float k, const std::shared_ptr<Material> &mat)
{
    fPos = pos;
    fK = k;
    fMat = mat;
}

bool XYRect::Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const
{
    float t = (fK - r.GetOrigin().z)/r.GetDirection().z;

    if(t < tMin || t > tMax ) return false;

    float x = r.GetOrigin().x + t*r.GetDirection().x;
    float y = r.GetOrigin().y + t*r.GetDirection().y;

    if(x < fPos.x || x > fPos.y || y < fPos.z || y > fPos.w)
        return false;

    record.uv = fm::math::vec2((x - fPos.x)/(fPos.y - fPos.x),(y - fPos.z)/(fPos.w - fPos.z) );
    record.t = t;
    record.material = fMat.get();
    record.p = r.PointAt(t);
    record.normal = vec3(0,0,1);
    return true;
}


XZRect::XZRect()
{

}

XZRect::XZRect(const fm::math::vec4 &pos, float k, const std::shared_ptr<Material> &mat)
{
    fPos = pos;
    fK = k;
    fMat = mat;
}

bool XZRect::Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const
{
    float t = (fK - r.GetOrigin().y)/r.GetDirection().y;
    if(t < tMin || t > tMax ) return false;

    float x = r.GetOrigin().x + t*r.GetDirection().x;
    float z = r.GetOrigin().z + t*r.GetDirection().z;

    if(x < fPos.x || x > fPos.y || z < fPos.z || z > fPos.w)
        return false;

    record.uv = fm::math::vec2((x - fPos.x)/(fPos.y - fPos.x),(z - fPos.z)/(fPos.w - fPos.z) );
    record.t = t;
    record.material = fMat.get();
    record.p = r.PointAt(t);
    record.normal = vec3(0,1,0);
    return true;
}

YZRect::YZRect()
{

}

YZRect::YZRect(const fm::math::vec4 &pos, float k, const std::shared_ptr<Material> &mat)
{
    fPos = pos;
    fK = k;
    fMat = mat;
}

bool YZRect::Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const
{
    float t = (fK - r.GetOrigin().x)/r.GetDirection().x;
    if(t < tMin || t > tMax ) return false;

    float y = r.GetOrigin().y + t*r.GetDirection().y;
    float z = r.GetOrigin().z + t*r.GetDirection().z;

    if(y < fPos.x || y > fPos.y || z < fPos.z || z > fPos.w)
        return false;

    record.uv = fm::math::vec2((y - fPos.x)/(fPos.y - fPos.x),(z - fPos.z)/(fPos.w - fPos.z) );
    record.t = t;
    record.material = fMat.get();
    record.p = r.PointAt(t);
    record.normal = vec3(1,0,0);
    return true;
}










