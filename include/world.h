#ifndef WORLD
#define WORLD
#include <vector>
#include "hitable.hpp"
#include <memory>

class World : public Hitable
{
public:
    void AddObject(std::unique_ptr<Hitable> &hitable);
    virtual bool Hit(const Ray &r, float tMin, float tMax, HitRecord &record) const;

private:
    std::vector<std::unique_ptr<Hitable>> objects;
};

#endif // WORLD

