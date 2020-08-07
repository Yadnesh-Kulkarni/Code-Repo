#include "Hitable_List.h"

bool Hitable_List::hit(const Ray& r, float tmin, float tmax, hit_record& rec) const
{
    hit_record tempRec;
    bool hit_anything = false;
    double closestSoFar = tmax;
    for (int i = 0; i < list_size; i++)
    {
        if (list[i]->hit(r, tmin, closestSoFar, tempRec)) {
            hit_anything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }
    return hit_anything;
}
