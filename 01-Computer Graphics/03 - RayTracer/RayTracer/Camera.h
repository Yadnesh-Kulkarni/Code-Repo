#pragma once
#include "Ray.h"
#include "Random.h"

vec3 random_in_unit_disk()
{
	vec3 p;
	do{
		p = 2.0f * vec3(random_double(), random_double(), 0.0f) - vec3(1.0f, 1.0f, 0.0f);
	} while (dot(p, p) >= 1);
	return p;
}

class Camera
{
public:
	Camera(vec3 lookfrom,vec3 lookat,vec3 vup,float vFov,float aspect,float aperture,float focus_dist) {
		lens_radius = aperture / 2;
		float theta = vFov * PI / 180;
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;

		origin = lookfrom;
		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);

		lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist * v - focus_dist * w;
		horizontal = 2.0 * half_width * focus_dist * u;
		vertical = 2.0 * half_height * focus_dist * v;
	}
	Ray get_ray(float a, float b) { 
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();
		return Ray(origin + offset , lower_left_corner + b * horizontal + a * vertical - origin - offset); };

	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lens_radius;
};

