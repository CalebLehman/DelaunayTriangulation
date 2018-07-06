#ifndef DELAUNAY_H
#define DELAUNAY_H

#include "Typedefine.h"

/* Final delaunay functions
 */
ExtremeEdge *delaunay_horizontal(Point *points_sorted[], size_t num_points, EdgeList *edge_list);
ExtremeEdge *delaunay_vertical(Point *points_sorted[], size_t num_points, EdgeList *edge_list);

/* Auxillary functions for delaunay functions
 */
Edge *makeLowerCommonTangent(Edge *left_edge, Edge *right_edge);
Edge *nextCrossEdge(Edge *base, EdgeList *edge_list);

void triangulateEmptyPolygon(Edge *e);
#endif
