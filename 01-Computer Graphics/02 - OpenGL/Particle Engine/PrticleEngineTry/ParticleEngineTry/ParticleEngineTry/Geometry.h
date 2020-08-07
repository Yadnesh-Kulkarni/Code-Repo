//#ifndef __GEOMETRY_H__
//#define __GEOMETRY_H__

/* SPHERE */
void draw_sphere();
void make_sphere(double radius, int slices, int stacks);
int get_index_count_sphere();
int get_vertex_count_sphere();
void print_sphere_data();

/* CYLINDER */
void draw_cylinder();
void make_cylinder(double radius, double height, int slices, int stacks);
int get_index_count_cylinder();
int get_vertex_count_cylinder();
void print_cylinder_data();

/* COMMON */
void deallocate_mesh();

//#endif

