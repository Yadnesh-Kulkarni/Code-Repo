#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES  1 // Include constants defined in math.h
#include <math.h>
#include <C:\glew\include\GL\glew.h>
#include <gl/gl.h>

#include "OGLPP.h"
#include "Geometry.h"

/* Global variable declarations */

/******************************************
 *                 SPHERE                 *
 ******************************************/
GLuint vao_sphere = 0;
GLuint vbo_position_sphere = 0;
GLuint vbo_normal_sphere = 0;
GLuint vbo_texture_sphere = 0;
GLuint vbo_index_sphere = 0;

float *vertices_sphere, *normals_sphere, *textures_sphere;
unsigned short  *elements_sphere;

unsigned int num_elements_sphere = 0;
unsigned int num_vertices_sphere = 0;

GLsizei num_parts_sphere;
GLsizei num_vertices_per_part_sphere;

/******************************************
 *                CYLINDER                *
 ******************************************/
GLuint vao_cylinder = 0;
GLuint vbo_position_cylinder = 0;
GLuint vbo_normal_cylinder = 0;
GLuint vbo_texture_cylinder = 0;
GLuint vbo_index_cylinder = 0;

float *vertices_cylinder, *normals_cylinder, *textures_cylinder;
unsigned short  *elements_cylinder;

unsigned int num_elements_cylinder = 0;
unsigned int num_vertices_cylinder = 0;

GLsizei num_parts_cylinder;
GLsizei num_vertices_per_part_cylinder;

/******************************************
 *                 SPHERE                 *
 ******************************************/

int get_index_count_sphere()
{
	/* code */
	return(num_elements_sphere);
}
    
int get_vertex_count_sphere()
{
	/* code */
	return(num_vertices_sphere);
}

void generate_sphere(float radius, int slices, int stacks, float **vertices, float **normals, float **textures, int *n_vert)
{
	/* function prototype */
	void circle_table(float **sint, float **cost, const int n, const bool half_circle);

	int i, j;
	int idx = 0; /* idx into vertex/normal buffer */
	int idy = 0; /* idy into texture buffer */
	float x ,y ,z;
	float ds = 1.0f / (float)(slices);
	float dt = 1.0f / (float)(stacks);
	float t = 1.0f;
	float s = 0.0f;
	
	/* Pre-computed circle */
	float *sint1, *cost1;
	float *sint2, *cost2;

	/* code */
	/* number of unique vertices */
	if (slices == 0 || stacks < 2)
	{
		/* nothing to generate */
		*n_vert = 0;
		num_vertices_sphere = *n_vert;
		return;
	}
	*n_vert = slices * (stacks - 1) + 2;
	num_vertices_sphere = *n_vert;
	if ((*n_vert) > 65535)
	{
		printf("%d Too many slices or stacks requested, indices will wrap\n", __LINE__);
		return;
	}
	
	/* precompute values on unit circle */
	circle_table(&sint1, &cost1, -slices, false);
	circle_table(&sint2, &cost2, stacks, true);
	
	/* Allocate vertex and normal buffers, bail out if memory allocation fails */
	*vertices = (float *)malloc((*n_vert) * 3 * sizeof(float));
	if (!(*vertices))
	{
		printf("%d Failed to allocate memory for vertices\n", __LINE__);
		return;
	}
	*normals  = (float *)malloc((*n_vert) * 3 * sizeof(float));
	if (!(*normals))
	{
		printf("%d Failed to allocate memory for normals\n", __LINE__);
		free(*vertices);
		*vertices = NULL;
		return;
	}
	*textures  = (float *)malloc((*n_vert) * 2 * sizeof(float));
	if (!(*textures))
	{
		printf("%d Failed to allocate memory for textures\n", __LINE__);
		free(*vertices);
		*vertices = NULL;
		free(*normals);
		*normals = NULL;
		return;
	}
	
	/* top */
	(*vertices)[0] = 0.0f;
	(*vertices)[1] = 0.0f;
	(*vertices)[2] = radius;
	(*normals)[0] = 0.0f;
	(*normals)[1] = 0.0f;
	(*normals)[2] = 1.0f;
	(*textures)[0] = s;
	(*textures)[1] = t;
	idx = 3;
	idy = 2;
	
	/* each stack */
	for(i = 1; i < stacks; i++)
	{
		s = 0.0f;
		t -= dt;
		
		for(j = 0; j < slices; j++, idx += 3, idy += 2)
		{
			x = cost1[j] * sint2[i];
			y = sint1[j] * sint2[i];
			z = cost2[i];
			
			(*vertices)[idx] = x * radius;
			(*vertices)[idx + 1] = y * radius;
			(*vertices)[idx + 2] = z * radius;
			(*normals)[idx] = x;
			(*normals)[idx + 1] = y;
			(*normals)[idx + 2] = z;
			(*textures)[idy] = s;
			(*textures)[idy + 1] = t;
			s += ds;
		}
	}
	
	/* bottom */
	(*vertices)[idx] =  0.0f;
	(*vertices)[idx + 1] =  0.0f;
	(*vertices)[idx + 2] = -radius;
	(*normals)[idx] =  0.0f;
	(*normals)[idx + 1] =  0.0f;
	(*normals)[idx + 2] = -1.0f;
	(*textures)[idy] = s;
	(*textures)[idy + 1] = t;
	
	/* Done creating vertices, release sin and cos tables */
	if (sint1)
	{
		free(sint1);
		sint1 = NULL;
	}
	if (cost1)
	{
    		free(cost1);
		cost1 = NULL;
	}
	if (sint2)
	{
		free(sint2);
		sint2 = NULL;
	}
	if (cost2)
	{
		free(cost2);
		cost2 = NULL;
	}
}

void draw_sphere()
{
	/* code */
        /* bind vao_sphere */
	glBindVertexArray(vao_sphere);

        /* draw */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index_sphere);

	for (int i = 0; i < num_parts_sphere; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, num_vertices_per_part_sphere, GL_UNSIGNED_SHORT, (GLvoid *)(sizeof(unsigned short) * i * num_vertices_per_part_sphere));
	}

        /* unbind vao_sphere */
	glBindVertexArray(0);
}

/*
 * Draws a solid sphere
 */
void make_sphere(double radius, int slices, int stacks)
{
	/* function prototype */
	void generate_sphere(float radius, int slices, int stacks, float **vertices, float **normals, float **textures, int *n_vert);
	void prepare_to_draw_sphere();
	void cleanup_sphere_mesh();

	int i, j, idx;
	int n_vert;
	unsigned short offset;

	/* code */
	/* Generate vertices and normals */
	generate_sphere(radius, slices, stacks, &vertices_sphere, &normals_sphere, &textures_sphere, &n_vert);
	printf("%d\n", __LINE__);
	
	if (n_vert == 0)
	{
		/* nothing to draw */
		return;
	}
	
	/* First, generate vertex index arrays for drawing with glDrawElement
	 * All stacks, including top and bottom are covered with a triangle
	 * strip.
	 */
	elements_sphere = (unsigned short *)malloc((slices + 1) * 2 * (stacks) * sizeof(unsigned short));
	if (!(elements_sphere))
	{
		printf("%d Failed to allocate memory for elements_sphere\n", __LINE__);
		cleanup_sphere_mesh();
		return;
	}

        /* top stack */
	for (j = 0, idx = 0;  j < slices;  j++, idx += 2)
	{
		elements_sphere[idx] = j + 1; /* 0 is top vertex, 1 is first for first stack */
		elements_sphere[idx + 1] = 0;
	}
	elements_sphere[idx] = 1; /* repeat first slice's idx for closing off shape */
	elements_sphere[idx+1] = 0;
	idx+=2;
	
	/* middle stacks: */
	/* Strip indices are relative to first index belonging to strip, NOT relative to first vertex/normal pair in array */
	for (i = 0; i < stacks - 2; i++, idx += 2)
	{
		offset = 1 + i * slices; /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
		for (j = 0; j < slices; j++, idx += 2)
		{
			elements_sphere[idx] = offset + j + slices;
			elements_sphere[idx + 1] = offset + j;
		}
		elements_sphere[idx] = offset + slices; /* repeat first slice's idx for closing off shape */
		elements_sphere[idx + 1] = offset;
	}
	
	/* bottom stack */
	offset = 1 + (stacks - 2) * slices; /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
	for ( j = 0; j < slices; j++, idx += 2)
	{
		elements_sphere[idx] = n_vert - 1; /* zero based index, last element in array (bottom vertex)... */
		elements_sphere[idx + 1] = offset + j;
	}
	elements_sphere[idx] = n_vert - 1; /* repeat first slice's idx for closing off shape */
	elements_sphere[idx + 1] = offset;
	num_elements_sphere = idx + 2;

	num_parts_sphere = stacks;
	num_vertices_per_part_sphere = (slices + 1) * 2;
	
	printf("%d\n", __LINE__);
	prepare_to_draw_sphere();
	printf("%d\n", __LINE__);
}

void prepare_to_draw_sphere()
{
	/* vao_sphere */
	glGenVertexArrays(1, &vao_sphere);
	glBindVertexArray(vao_sphere);

        /* vbo for position */
	glGenBuffers(1, &vbo_position_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_sphere);
	glBufferData(GL_ARRAY_BUFFER, (num_vertices_sphere * 3 * sizeof(float)), vertices_sphere, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0); /* Unbind with vbo_position */

        /* vbo for normals */
	glGenBuffers(1, &vbo_normal_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_sphere);
	glBufferData(GL_ARRAY_BUFFER, (num_vertices_sphere * 3 * sizeof(float)), normals_sphere, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0); /* Unbind with vbo_normal */

        /* vbo for texture */
	glGenBuffers(1, &vbo_texture_sphere);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_sphere);
	glBufferData(GL_ARRAY_BUFFER, (num_vertices_sphere * 2 * sizeof(float)), textures_sphere, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); /* Unbind with vbo_texture */

        /* vbo for index */
	glGenBuffers(1, &vbo_index_sphere);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index_sphere);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (num_elements_sphere * sizeof(unsigned short)), elements_sphere, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); /* Unbind with vbo_index */

	glBindVertexArray(0); /* Unbind with vao_sphere */
}

void print_sphere_data()
{
	/* function prototype */
	void print_array_data(FILE *gp_file, float *array, int n_coords, const char *array_name, int num_vertices);
	void print_elements(FILE *gp_file, unsigned short *array, const char *array_name, int num_elements);

	FILE *gp_file = NULL;

	/* code */
	gp_file = fopen("SphereMesh.txt", "w");
	if (!gp_file)
	{
		printf("Failed to create SphereMesh.txt\n");
		return;
	}

	print_array_data(gp_file, vertices_sphere, 3, "sphere_vertices", num_vertices_sphere);
	print_array_data(gp_file, normals_sphere, 3, "sphere_normals", num_vertices_sphere);
	print_array_data(gp_file, textures_sphere, 2, "sphere_textures", num_vertices_sphere);
	print_elements(gp_file, elements_sphere, "sphere_elements", num_elements_sphere);

	if (gp_file)
	{
		fclose(gp_file);
		gp_file = NULL;
	}
}

void cleanup_sphere_mesh()
{
	/* cleanup allocated memory */
	if (elements_sphere)
	{
		free(elements_sphere);
		elements_sphere = NULL;
	}
	if (vertices_sphere)
	{
		free(vertices_sphere);
		vertices_sphere = NULL;
	}
	if (normals_sphere)
	{
		free(normals_sphere);
		normals_sphere = NULL;
	}
	if (textures_sphere)
	{
		free(textures_sphere);
		textures_sphere = NULL;
	}
}

/******************************************
 *                CYLINDER                *
 ******************************************/
int get_index_count_cylinder()
{
	/* code */
	return(num_elements_cylinder);
}
    
int get_vertex_count_cylinder()
{
	/* code */
	return(num_vertices_cylinder);
}

void generate_cylinder(float radius, float height, int slices, int stacks, float **vertices, float **normals, float **textures, int *n_vert)
{
	/* function prototype */
	void circle_table(float **sint, float **cost, const int n, const bool half_circle);

	int i, j;
	int idx = 0; /* idx into vertex/normal buffer */
	int idy = 0; /* idy into texture buffer */
	float x, y, z;
	float ds = 1.0f / (float)(slices);
	float dt = 1.0f / (float)(stacks);
	float t = 1.0f;
	float s = 0.0f;
	const float z_step = height / ((stacks > 0) ? stacks : 1);
	
	/* Pre-computed circle */
	float *sint, *cost;

	/* code */
	/* number of unique vertices */
	if (slices == 0 || stacks < 1)
	{
		/* nothing to generate */
		*n_vert = 0;
		num_vertices_cylinder = *n_vert;
		return;
	}
	*n_vert = slices * (stacks + 3) + 2; /* need two extra stacks for closing off top and bottom with correct normals */
	num_vertices_cylinder = *n_vert;
	if ((*n_vert) > 65535)
	{
		printf("%d Too many slices or stacks requested, indices will wrap",
			__LINE__);
		return;
	}
	
	/* precompute values on unit circle */
	circle_table(&sint, &cost, -slices, false);

	/* Allocate vertex and normal buffers, bail out if memory allocation fails */
	*vertices = (float *)malloc((*n_vert) * 3 * sizeof(float));
	if (!(*vertices))
	{
		printf("%d Failed to allocate memory for vertices\n", __LINE__);
		return;
	}
	*normals  = (float *)malloc((*n_vert) * 3 * sizeof(float));
	if (!(*normals))
	{
		printf("%d Failed to allocate memory for normals\n", __LINE__);
		free(*vertices);
		*vertices = NULL;
		return;
	}
	*textures  = (float *)malloc((*n_vert) * 2 * sizeof(float));
	if (!(*textures))
	{
		printf("%d Failed to allocate memory for textures\n", __LINE__);
		free(*vertices);
		*vertices = NULL;
		free(*normals);
		*normals = NULL;
		return;
	}
	
	z = 0.0f;
	/* top on Z-axis */
	(*vertices)[0] = 0.0f;
	(*vertices)[1] = 0.0f;
	(*vertices)[2] = 0.0f;
	(*normals)[0] = 0.0f;
	(*normals)[1] = 0.0f;
	(*normals)[2] = -1.0f;
	(*textures)[0] = s;
	(*textures)[1] = t;
	idx = 3;
	idy = 2;
		
	/* other on top (get normals right) */
	for(j = 0; j < slices; j++, idx += 3, idy += 2)
	{
		x = cost[j];
		y = sint[j];

		(*vertices)[idx] = x * radius;
		(*vertices)[idx + 1] = y * radius;
		(*vertices)[idx + 2] = z;
		(*normals)[idx] = 0.0;
		(*normals)[idx + 1] = 0.0f;
		(*normals)[idx + 2] = -1.0f;
		(*textures)[idy] = s;
		(*textures)[idy + 1] = t;
		s += ds;
	}
	
	/* each stack */
	for(i = 0; i < stacks + 1; i++)
	{
		s = 0.0f;
		t -= dt;
		
		for(j = 0; j < slices; j++, idx += 3, idy += 2)
		{
			x = cost[j];
			y = sint[j];

			(*vertices)[idx] = x * radius;
			(*vertices)[idx + 1] = y * radius;
			(*vertices)[idx + 2] = z;
			(*normals)[idx] = x;
			(*normals)[idx + 1] = y;
			(*normals)[idx + 2] = 0.0f;
			(*textures)[idy] = s;
			(*textures)[idy + 1] = t;
			s += ds;
		}

		z += z_step;
	}
	
	/* other on bottom (get normals right) */
	z -= z_step;
	for(j = 0; j < slices; j++, idx += 3, idy += 2)
	{
		x = cost[j];
		y = sint[j];

		(*vertices)[idx] = x * radius;
		(*vertices)[idx + 1] = y * radius;
		(*vertices)[idx + 2] = z;
		(*normals)[idx] = 0.0;
		(*normals)[idx + 1] = 0.0f;
		(*normals)[idx + 2] = 1.0f;
		(*textures)[idy] = s;
		(*textures)[idy + 1] = t;
		s += ds;
	}
	
	/* bottom */
	(*vertices)[idx] =  0.0f;
	(*vertices)[idx + 1] =  0.0f;
	(*vertices)[idx + 2] = height;
	(*normals)[idx] =  0.0f;
	(*normals)[idx + 1] =  0.0f;
	(*normals)[idx + 2] = 1.0f;
	(*textures)[idy] = s;
	(*textures)[idy + 1] = t;
	
	/* Done creating vertices, release sin and cos tables */
	if (sint)
	{
		free(sint);
		sint = NULL;
	}
	if (cost)
	{
    		free(cost);
		cost = NULL;
	}
}

void draw_cylinder()
{
	/* code */
        /* bind vao_cylinder */
	glBindVertexArray(vao_cylinder);

        /* draw */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index_cylinder);

	for (int i = 0; i < num_parts_cylinder; i++)
	{
		glDrawElements(GL_TRIANGLE_STRIP, num_vertices_per_part_cylinder, GL_UNSIGNED_SHORT, (GLvoid *)(sizeof(unsigned short) * i * num_vertices_per_part_cylinder));
	}

        /* unbind vao_cylinder */
	glBindVertexArray(0);
}

/*
 * Draws a solid cylinder
 */
void make_cylinder(double radius, double height, int slices, int stacks)
{
	/* function prototype */
	void generate_cylinder(float radius, float height, int slices, int stacks, float **vertices, float **normals, float **textures, int *n_vert);
	void prepare_to_draw_cylinder();
	void cleanup_cylinder_mesh();

	int i, j, idx;
	int n_vert;
	unsigned short offset;

	/* code */
	/* Generate vertices and normals */
	generate_cylinder(radius, height, slices, stacks, &vertices_cylinder, &normals_cylinder, &textures_cylinder,&n_vert);
	
	if (n_vert==0)
	{
		/* nothing to draw */
		return;
	}

	/* First, generate vertex index arrays for drawing with glDrawElement
	 * All stacks, including top and bottom are covered with a triangle
	 * strip.
	 */
	elements_cylinder = (unsigned short *)malloc((slices + 1) * 2 * (stacks + 2) * sizeof(unsigned short)); /* stacks +2 because of closing off bottom and top */
	if (!(elements_cylinder))
	{
		printf("%d Failed to allocate memory for elements_cylinder\n", __LINE__);
		cleanup_cylinder_mesh();
		return;
	}

        /* top stack */
	for (j = 0, idx = 0;  j < slices;  j++, idx += 2)
	{
		elements_cylinder[idx] = 0;
		elements_cylinder[idx + 1] = j + 1; /* 0 is top vertex, 1 is first for first stack */
	}
	elements_cylinder[idx] = 0; /* repeat first slice's idx for closing off shape */
	elements_cylinder[idx + 1] = 1;
	idx += 2;
	
	/* middle stacks: */
	/* Strip indices are relative to first index belonging to strip, NOT relative to first vertex/normal pair in array */
	for (i = 0; i < stacks; i++, idx += 2)
	{
		offset = 1 + (i + 1) * slices; /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
		for (j = 0; j < slices; j++, idx += 2)
		{
			elements_cylinder[idx] = offset + j;
			elements_cylinder[idx + 1] = offset + j + slices;
		}
		elements_cylinder[idx] = offset; /* repeat first slice's idx for closing off shape */
		elements_cylinder[idx + 1] = offset + slices;
	}
	
	/* bottom stack */
	offset = 1 + (stacks + 2) * slices; /* triangle_strip indices start at 1 (0 is top vertex), and we advance one stack down as we go along */
	for ( j = 0; j < slices; j++, idx += 2)
	{
		elements_cylinder[idx] = offset + j;
		elements_cylinder[idx + 1] = n_vert - 1; /* zero based index, last element in array (bottom vertex)... */
	}
	elements_cylinder[idx] = offset;
	elements_cylinder[idx + 1] = n_vert - 1; /* repeat first slice's idx for closing off shape */

	num_elements_cylinder = idx + 2;

	num_parts_cylinder = stacks + 2;
	num_vertices_per_part_cylinder = (slices + 1) * 2;
	
	prepare_to_draw_cylinder();
}

void prepare_to_draw_cylinder()
{
	/* vao_cylinder */
	glGenVertexArrays(1, &vao_cylinder);
	glBindVertexArray(vao_cylinder);

        /* vbo for position */
	glGenBuffers(1, &vbo_position_cylinder);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cylinder);
	glBufferData(GL_ARRAY_BUFFER, (num_vertices_cylinder * 3 * sizeof(float)), vertices_cylinder, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0); /* Unbind with vbo_position */

        /* vbo for normals */
	glGenBuffers(1, &vbo_normal_cylinder);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_cylinder);
	glBufferData(GL_ARRAY_BUFFER, (num_vertices_cylinder * 3 * sizeof(float)), normals_cylinder, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0); /* Unbind with vbo_normal */

        /* vbo for texture */
	glGenBuffers(1, &vbo_texture_cylinder);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_cylinder);
	glBufferData(GL_ARRAY_BUFFER, (num_vertices_cylinder * 2 * sizeof(float)), textures_cylinder, GL_STATIC_DRAW);

	glVertexAttribPointer(VDG_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(VDG_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); /* Unbind with vbo_texture */

        /* vbo for index */
	glGenBuffers(1, &vbo_index_cylinder);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index_cylinder);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (num_elements_cylinder * sizeof(unsigned short)), elements_cylinder, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); /* Unbind with vbo_index */

	glBindVertexArray(0); /* Unbind with vao_sphere */
}

void print_cylinder_data()
{
	/* function prototype */
	void print_array_data(FILE *gp_file, float *array, int n_coords, const char *array_name, int num_vertices);
	void print_elements(FILE *gp_file, unsigned short *array, const char *array_name, int num_elements);

	FILE *gp_file = NULL;

	/* code */
	gp_file = fopen("CylinderMesh.txt", "w");
	if (!gp_file)
	{
		printf("Failed to create CylinderMesh.txt\n");
		return;
	}

	print_array_data(gp_file, vertices_cylinder, 3, "cylinder_vertices", num_vertices_cylinder);
	print_array_data(gp_file, normals_cylinder, 3, "cylinder_normals", num_vertices_cylinder);
	print_array_data(gp_file, textures_cylinder, 2, "cylinder_textures", num_vertices_cylinder);
	print_elements(gp_file, elements_cylinder, "cylinder_elements", num_elements_cylinder);

	if (gp_file)
	{
		fclose(gp_file);
		gp_file = NULL;
	}
}

void cleanup_cylinder_mesh()
{
	/* cleanup allocated memory */
	if (elements_cylinder)
	{
		free(elements_cylinder);
		elements_cylinder = NULL;
	}
	if (vertices_cylinder)
	{
		free(vertices_cylinder);
		vertices_cylinder = NULL;
	}
	if (normals_cylinder)
	{
		free(normals_cylinder);
		normals_cylinder = NULL;
	}
	if (textures_cylinder)
	{
		free(textures_cylinder);
		textures_cylinder = NULL;
	}
}

/******************************************
 *                 COMMON                 *
 ******************************************/

void print_array_data(FILE *gp_file, float *array, int n_coords, const char *array_name, int num_vertices)
{
	fprintf(gp_file, "GLfloat %s[] =\n", array_name);
	fprintf(gp_file, "{\n");
	
	for (int i = 0; i < num_vertices * n_coords; i += n_coords)
	{
		fprintf(gp_file, "	%lf, ", *(array + i));
		if (n_coords == 3)
		{
			fprintf(gp_file, "%lf, ", *(array + i + 1));
			fprintf(gp_file, "%lf,\n", *(array + i + 2));
		}
		else
		{
			fprintf(gp_file, "%lf,\n", *(array + i + 1));
		}
	}
	fprintf(gp_file, "};\n");
	fprintf(gp_file, "\n");
}

void print_elements(FILE *gp_file, unsigned short *array, const char *array_name, int num_elements)
{
	fprintf(gp_file, "GLfloat %s[] =\n", array_name);
	fprintf(gp_file, "{\n");
	for (int i = 0; i < num_elements; i += 3)
	{
		fprintf(gp_file, "	%d, ", *(array + i));
		fprintf(gp_file, "%d, ", *(array + i + 1));
		fprintf(gp_file, "%d,\n", *(array + i + 2));
	}
	fprintf(gp_file, "};\n");
	fprintf(gp_file, "\n");
}

/* -- Now the various non-polyhedra (shapes involving circles) -- */
/*
 * Compute lookup table of cos and sin values forming a circle
 * (or half circle if half_circle==TRUE)
 *
 * Notes:
 *    It is the responsibility of the caller to free these tables
 *    The size of the table is (n+1) to form a connected loop
 *    The last entry is exactly the same as the first
 *    The sign of n can be flipped to get the reverse loop
 */
void circle_table(float **sint, float **cost, const int n, const bool half_circle)
{
	int i;

	/* code */
	/* Table size, the sign of n flips the circle direction */
	const int size = abs(n);
	
	/* Determine the angle between samples */
	const float angle = (half_circle ? 1 : 2) * (float)M_PI / (float)((n == 0) ? 1 : n);
	
	/* Allocate memory for n samples, plus duplicate of first entry at the end */
	*sint = (float *)malloc(sizeof(float) * (size+1));
	if (!(sint))
	{
		printf("%d Failed to allocate memory for sint\n", __LINE__);
		return;
	}
	*cost = (float *)malloc(sizeof(float) * (size+1));
	if (!(cost))
	{
		printf("%d Failed to allocate memory for cost\n", __LINE__);
		free(*sint);
		*sint = NULL;
		return;
	}
	
	/* Compute cos and sin around the circle */
	(*sint)[0] = 0.0f;
	(*cost)[0] = 1.0f;
	
	for (i = 1; i < size; i++)
	{
		(*sint)[i] = (float)sin(angle * i);
		(*cost)[i] = (float)cos(angle * i);
	}
	
	if (half_circle)
	{
		(*sint)[size] =  0.0f;  /* sin PI */
		(*cost)[size] = -1.0f;  /* cos PI */
	}
	else
	{
		/* Last sample is duplicate of the first (sin or cos of 2 PI) */
		(*sint)[size] = (*sint)[0];
		(*cost)[size] = (*cost)[0];
	}
}

void deallocate_mesh()
{
	/* function prototype */
	void cleanup_sphere_mesh();
	void cleanup_cylinder_mesh();

        /* code */
	/* SPHERE */
        if(vao_sphere)
        {
	    glDeleteVertexArrays(1, &vao_sphere);
            vao_sphere = 0;
        }
        
        if(vbo_index_sphere)
        {
	    glDeleteBuffers(1, &vbo_index_sphere);
            vbo_index_sphere = 0;
        }
        
        if(vbo_texture_sphere)
        {
	    glDeleteBuffers(1, &vbo_texture_sphere);
            vbo_texture_sphere = 0;
        }
        
        if(vbo_normal_sphere)
        {
	    glDeleteBuffers(1, &vbo_normal_sphere);
            vbo_normal_sphere = 0;
        }
        
        if(vbo_position_sphere)
        {
	    glDeleteBuffers(1, &vbo_position_sphere);
            vbo_position_sphere = 0;
        }

	cleanup_sphere_mesh();
	
	/* CYLINDER */
        if(vao_cylinder)
        {
	    glDeleteVertexArrays(1, &vao_cylinder);
            vao_cylinder = 0;
        }
        
        if(vbo_index_cylinder)
        {
	    glDeleteBuffers(1, &vbo_index_cylinder);
            vbo_index_cylinder = 0;
        }
        
        if(vbo_texture_cylinder)
        {
	    glDeleteBuffers(1, &vbo_texture_cylinder);
            vbo_texture_cylinder = 0;
        }
        
        if(vbo_normal_cylinder)
        {
	    glDeleteBuffers(1, &vbo_normal_cylinder);
            vbo_normal_cylinder = 0;
        }
        
        if(vbo_position_cylinder)
        {
	    glDeleteBuffers(1, &vbo_position_cylinder);
            vbo_position_cylinder = 0;
        }

	cleanup_cylinder_mesh();
}
