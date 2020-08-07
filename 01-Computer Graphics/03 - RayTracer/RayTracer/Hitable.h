#pragma once
#include "Ray.h"
//#include "Material.h"
class Material;

struct hit_record {
	float t; //Hit Point
	vec3 p;//Point At Param On Ray
	vec3 normal;//Normal At Hitpoint
	Material* mat_ptr;
};

class Hitable
{
public:
	virtual bool hit(const Ray& r, float t_min, float t_max, hit_record& rec) const = 0;
};

