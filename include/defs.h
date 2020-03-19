#ifndef TYPEDEFINE_H
#define TYPEDEFINE_H

#include <stdlib.h>

typedef struct Point Point;
typedef struct Edge Edge;
typedef struct ExtremeEdge ExtremeEdge;
typedef long VALUE;
#define VALUE_SPEC "%ld"
typedef struct EdgeList EdgeList;
typedef struct PointList PointList;

/* By convention:
 *  Positive rotation about a point
 *  is counter-clockwise
 *  It follows that positive rotation
 *  about exterior of polygonal boundary
 *  is clockwise and about interior is
 *  counter-clockwise
 */
struct Point
{
    VALUE x;
    VALUE y;
    Edge *e;
};

struct Edge
{
    Point *orig;
    Edge *oprev;
    Edge *dnext;
    Edge *twin;
};

struct ExtremeEdge
{
    Edge *left_edge_ccw;
    Edge *right_edge_cw;
    Edge *bottom_edge_ccw;
    Edge *top_edge_cw;
};

struct PointList
{
    Point *points;
    Point **unused_points;
    size_t idx;
    size_t size;
};

struct EdgeList
{
    Edge *edges;
    Edge **unused_edges;
    size_t idx;
    size_t size;
};

/* Usefull macros
 */
#define SWAP(a, b, T) {T temp_swap_var = (a); (a) = (b); (b) = temp_swap_var;}

#endif
