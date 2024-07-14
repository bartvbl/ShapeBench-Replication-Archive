#pragma once
#ifndef RSW_PRIMITIVES_H
#define RSW_PRIMITIVES_H

#include <rsw_math.h>
using rsw::vec3;
using rsw::ivec3;
using rsw::vec4;
using rsw::vec2;
using rsw::mat4;
using rsw::mat3;



#include <vector>
using std::vector;



void make_box(vector<vec3>& verts, vector<ivec3>& tris, vector<vec2>& tex, float dimX, float dimY, float dimZ, bool plane=false, ivec3 seg = ivec3(1,1,1), vec3 origin= vec3(0,0,0));

void make_cylinder(vector<vec3>& verts, vector<ivec3>& tris, vector<vec2>& tex, float radius, float height, size_t slices);
void make_cylindrical(vector<vec3>& verts, vector<ivec3>& tris, vector<vec2>& tex, float radius, float height, size_t slices, size_t stacks, float top_radius);

//plane faces in direction of v2 x v1 (cross product of v2 and v1) ie v1 = -z and v2 = x, plane would face up/y
//textured with v1 = x, v2 = y so put corner in upper left to get it upright.
//This is because OpenGL treats 0,0 as first pixel of image and for images/framebuffers in memory, that's the top left corner.
void make_plane(vector<vec3>& verts, vector<ivec3>& tris, vector<vec2>& tex, vec3 corner, vec3 v1, vec3 v2, size_t dimV1, size_t dimV2, bool tile=false);

void make_sphere(vector<vec3>& verts, vector<ivec3>& tris, vector<vec2>& tex, float radius, size_t slices, size_t stacks);

void make_torus(vector<vec3>& verts, vector<ivec3>& tris, vector<vec2>& tex, float major_r, float minor_r, size_t major_slices, size_t minor_slices);


void make_cone(vector<vec3>& verts, vector<ivec3>& tris, vector<vec2>& tex, float radius, float height, size_t slices, size_t stacks, bool flip=false);


void expand_verts(vector<vec3>& draw_verts, vector<vec3>& verts, vector<ivec3>& triangles);
void expand_tex(vector<vec2>& draw_tex, vector<vec2>& tex, vector<ivec3>& triangles);


void make_tetrahedron(vector<vec3>& verts, vector<ivec3>& tris);
void make_cube(vector<vec3>& verts, vector<ivec3>& tris);
void make_octahedron(vector<vec3>& verts, vector<ivec3>& tris);
void make_dodecahedron(vector<vec3>& verts, vector<ivec3>& tris);
void make_icosahedron(vector<vec3>& verts, vector<ivec3>& tris);


#endif

