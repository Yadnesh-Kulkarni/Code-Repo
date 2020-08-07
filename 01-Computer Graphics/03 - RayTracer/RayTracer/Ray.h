#pragma once
#include "vec3.h"
#define PI 3.14159265
class Ray
{
public:
	vec3 A;//Origin of Ray
	vec3 B;//Direction of Ray

	Ray() {}
	Ray(const vec3& a, const vec3& b) { A = a; B = b; }
	vec3 origin() const { return A; }
	vec3 direction() const { return B; }
	vec3 point_at_parameter(float t) const { return (A + t * B); }
};
