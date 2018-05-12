#ifndef PLANE_HPP
#define PLANE_HPP
#include "hitable.hpp"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

class DiskPlane : public Hitable
{
public:
    DiskPlane(fm::math::vec3 normal, fm::math::vec3 origin, float radius, const std::shared_ptr<Material> &mat);
    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const;

    float fRadius;
    fm::math::vec3 fNormal;
    fm::math::vec3 fOrigin;
    std::shared_ptr<Material> fMat;

};

class RectPlane : public Hitable
{
public:
    RectPlane(fm::math::vec3 origin, fm::math::vec3 normal, fm::math::vec2 size, const std::shared_ptr<Material> &mat);
    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const;

    fm::math::vec2 fSize;
    fm::math::vec3 fNormal;
    fm::math::vec3 fOrigin;
    std::shared_ptr<Material> fMat;
private:
    fm::math::vec3 u;
    fm::math::vec3 v;
};

class XYRect : public Hitable
{
public:
    XYRect();
    XYRect(const fm::math::vec4 &pos, float k, const std::shared_ptr<Material> &mat);
    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const;

    fm::math::vec4 fPos;
    float fK;
    std::shared_ptr<Material> fMat;
};

class XZRect : public Hitable
{
public:
    XZRect();
    XZRect(const fm::math::vec4 &pos, float k, const std::shared_ptr<Material> &mat);
    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const;

    fm::math::vec4 fPos;
    float fK;
    std::shared_ptr<Material> fMat;
};

class YZRect : public Hitable
{
public:
    YZRect();
    YZRect(const fm::math::vec4 &pos, float k, const std::shared_ptr<Material> &mat);
    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const;

    fm::math::vec4 fPos;
    float fK;
    std::shared_ptr<Material> fMat;
};


#endif // PLANE_HPP

