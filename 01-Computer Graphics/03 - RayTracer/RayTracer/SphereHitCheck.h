#pragma once
#include "Hitable.h"
class SphereHitCheck : public Hitable
{
public:
    SphereHitCheck() {}
    SphereHitCheck(vec3 cen, float r, Material *mat) : center(cen), radius(r), m(mat) {};

    virtual bool hit(const Ray& r, float tmin, float tmax, hit_record& rec) const;

    vec3 center;
    float radius;
    Material* m;
};
