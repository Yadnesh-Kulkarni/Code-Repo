#include <iostream>
#include <float.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "SphereHitCheck.h"
#include "Hitable_List.h"
#include "Camera.h"
#include "Random.h"
#include "Material.h"

#define WIDTH 1280
#define HEIGHT 720

vec3 color(const Ray& r,Hitable *world,int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001F, 9999999.0F, rec))
	{
		Ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * color(scattered, world, depth + 1);
		}
		else
		{
			return vec3(0, 0, 0);
		}
	}
	else
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * vec3(1.0f, 1.0f, 1.0f) + t * vec3(0.5f, 0.7f, 1.0f);
	}
}

Hitable* RandomScene()
{
	int n = 500;
	Hitable** list = new Hitable*[n+1];

	list[0] = new SphereHitCheck(vec3(0, -1000, 0), 1000, new LambertianDiffuse(vec3(0.5f, 0.5f, 0.5f)));
	int i = 1;
	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			float chose_mat = random_double();
			vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9f * random_double());
			if ((center - vec3(4.0f, 0.2f, 0.0f)).length() > 0.9f)
			{
				if (chose_mat < 0.8) // Sphere
				{
					list[i] = new SphereHitCheck(center, 0.2f, new LambertianDiffuse(vec3(random_double(), random_double(), random_double())));
					i++;
				}
				else if (chose_mat < 0.95) // Metal
				{
					list[i] = new SphereHitCheck(center, 0.2f, new Metal(vec3(0.5f * (1 + random_double()), 0.5f * (1 + random_double()), 0.5f * (1 + random_double())), 0.5 * random_double()));
					i++;
				}
				else
				{
					list[i] = new SphereHitCheck(center, 0.2f, new Dielectric(1.5f));
					i++;
				}
			}
		}
	}

	list[i++] = new SphereHitCheck(vec3(0.0f, 1.0f, 0.0f), 1.0f, new Dielectric(1.5f));
	list[i++] = new SphereHitCheck(vec3(-4.0f, 1.0f, 0.0f), 1.0f, new LambertianDiffuse(vec3(0.4f, 0.3, 0.2f)));
	list[i++] = new SphereHitCheck(vec3(4.0f, 1.0f, 0.0f), 1.0f, new Metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));

	return new Hitable_List(list, i);
}

int main()
{
	unsigned char* pData;
	pData = (unsigned char *)malloc(WIDTH * HEIGHT * 3);
	//std::cout << "P3\n" << WIDTH << " " << HEIGHT << "\n255\n";
	int ns = 100;

	/*Hitable* list[5];
	list[0] = new SphereHitCheck(vec3(0.0f, 0.0f, -1.0f), 0.5f, new LambertianDiffuse(vec3(0.8f, 0.3f, 0.3f)));
	list[1] = new SphereHitCheck(vec3(0.0f, -100.5f, -1.0f), 100.0f, new LambertianDiffuse(vec3(0.8f,0.8f,0.0f)));
	list[2] = new SphereHitCheck(vec3(1.0f, 0.0f, -1.0f), 0.5f, new Metal(vec3(0.8f, 0.6f, 0.2f),0.3f));
	list[3] = new SphereHitCheck(vec3(-1.0f, 0.0f, -1.0f), 0.5f, new Dielectric(1.5f));
	list[4] = new SphereHitCheck(vec3(-1.0f, 0.0f, -1.0f), -0.45f, new Dielectric(1.5f));*/
	
	Hitable* world = RandomScene();

	vec3 lookfrom(13.0f, 2.0f, 3.0f);
	vec3 lookat(0.0f, 0.0f, 0.0f);
	vec3 vup(0.0f, 1.0f, 0.0f);
	float dist_focus = 10.0f;
	float aperture = 0.1f;
	float fov = 20;
	float aspect = (float)WIDTH / (float)HEIGHT;

	Camera cam(lookfrom,lookat,vup,fov,aspect,aperture,dist_focus);
	int index = 0;
	for (int i = HEIGHT - 1; i >= 0; i--)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			vec3 col(0.0f, 0.0f, 0.0f);
			for (int n = 0; n < ns; n++)
			{
				float u = float(i + random_double()) / HEIGHT;
				float v = float(j + random_double()) / WIDTH;
				Ray r = cam.get_ray(u, v);

				vec3 p = r.point_at_parameter(2.0f);
				col = col + color(r, world,0);
			}
			col = col / float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99f * col[0]);
			int ig = int(255.99f * col[1]);
			int ib = int(255.99f * col[2]);

			pData[index++] = (unsigned char)ir;
			pData[index++] = (unsigned char)ig;
			pData[index++] = (unsigned char)ib;
			//std::cout << ir << " " << ig << " " << ib << "\n";
		}
	}

	stbi_write_jpg("15-FinalScene.jpg", WIDTH, HEIGHT, 3, pData, WIDTH * 3);

	free(pData);
	std::cout << "Successful" << std::endl; 
}