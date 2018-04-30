#include "world.h"

void World::AddObject(std::unique_ptr<Hitable> &hitable)
{
    objects.push_back(std::move(hitable));
}

bool World::Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const
{
    HitRecord tempRecord;
    bool hasHit = false;
    float closestSoFar = tMax;
    for(auto &object : objects)
    {
        if(object->Hit(r, tMin, closestSoFar, tempRecord))
        {
            hasHit = true;
            closestSoFar = tempRecord.t;
            record = tempRecord;
        }
    }
    return hasHit;
}
