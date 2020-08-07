#pragma once
#include "Hitable.h"
class Hitable_List : public Hitable
{
public:
    Hitable_List() {}
    Hitable_List(Hitable** l, int n) { list = l; list_size = n; }
    virtual bool hit(const Ray& r, float tmin, float tmax, hit_record& rec) const;
    Hitable** list;
    int list_size;
};

