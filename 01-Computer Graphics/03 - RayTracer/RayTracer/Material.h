#pragma once
#include "Hitable.h"
#include "Random.h"
#include "Ray.h"

struct hit_record;

class Material
{
public:
	virtual bool scatter(const Ray& r, const hit_record& rec, vec3& attenuation, Ray& scattered) const = 0;
};

float schlick(float cosine, float ref_idx)
{
    float r = (1 - ref_idx) / (1 + ref_idx);
    r = r * r;
    return r + (1 - r) * pow((1 - cosine), 5);
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1.0f - dt * dt);
    if (discriminant > 0)
    {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    else
    {
        return false;
    }
}

vec3 unit_random_in_sphere()
{
	vec3 p;
	do {
		p = 2.0 * vec3(random_double(), random_double(), random_double()) - vec3(1, 1, 1);
	} while (p.squaredLength() >= 1.0);

	return p;
}

vec3 reflect(const vec3& a, const vec3& b)
{
	return a - 2 * dot(a, b) * b;
}

class LambertianDiffuse :
    public Material
{
public:
    LambertianDiffuse(const vec3& a) : albedo(a) {}
    virtual bool scatter(const Ray& r_in, const hit_record& rec, vec3& attenuation, Ray& scattered) const {
        vec3 target = rec.p + rec.normal + unit_random_in_sphere();
        scattered = Ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }

    vec3 albedo;
};

class Metal :
    public Material
{
public:
    Metal(const vec3& a, float f) : albedo(a) { if (f < 1)fuzziness = f; else fuzziness = 1; }
    vec3 albedo;
    float fuzziness;

    virtual bool scatter(const Ray& r, const hit_record& rec, vec3& attenuation, Ray& scattered) const {
        vec3 reflected = reflect(unit_vector(r.direction()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzziness*unit_random_in_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
};

class Dielectric : public Material
{
public:
    Dielectric(float ref) : ref_idx(ref) {}
    float ref_idx;

    virtual bool scatter(const Ray& r, const hit_record& rec, vec3& attenuation, Ray& scattered) const {
        vec3 outward_normal;
        vec3 reflected = reflect(r.direction(), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0f, 1.0f, 1.0f);
        float reflect_prob;
        float cosine;
        vec3 refracted;
        if (dot(r.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx * dot(r.direction(), rec.normal) / r.direction().length();
            //cosine = dot(r.direction(), rec.normal) / r.direction().length();
            //cosine = sqrt(1 - ref_idx * ref_idx * (1 - cosine * cosine));
        }
        else
        {
            outward_normal = rec.normal;
            ni_over_nt = 1 / ref_idx;
            cosine = -dot(r.direction(), rec.normal) / r.direction().length();
        }

        if (refract(r.direction(), outward_normal, ni_over_nt, refracted))
        {
            reflect_prob = schlick(cosine, ref_idx);
        }
        else
        {
           // scattered = Ray(rec.p, reflected);
            reflect_prob = 1.0f;
        }

        if (random_double() < reflect_prob)
        {
            scattered = Ray(rec.p, reflected);
        }
        else
        {
            scattered = Ray(rec.p, refracted);
        }
        return true;
    }
};
