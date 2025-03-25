#ifndef STRUCT_H
#define STRUCT_H

#include "global.h"
#include "triangles.h"

void push_into_stack(StackNode **start, dim_t value, CubeVertex vert);
void free_stack(StackNode **start);
CubeVertex *get_coordinate_by_idx(StackNode *start, int idx);
dim_t *get_value_by_idx(StackNode *start, int idx);
int push_vertex(VertexNode **start, TriangleVertex *m_vertex, int *vertex_counter);
void print_vertex_list(VertexNode *start);
void push_triangle(Polyhedra **p, Triangle *triangle, int *vertex_counter);
void print_triangle_list(TriangleNode *start);

#endif // STRUCT_H