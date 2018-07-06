#ifndef HELPER_H
#define HELPER_H

#include "Typedefine.h"

/* Point functions
 */
Point *makePoint(VALUE x, VALUE y);
void destroyPoint(Point *p, EdgeList *edge_list);
int compareXY(Point *a, Point *b);
int compareYX(Point *a, Point *b);
void freePoints(Point *point_list[], size_t num_points, EdgeList *edge_list);

/* Edge functions
 */
Edge *getEdge(EdgeList *edge_list);
void freeEdge(EdgeList *edge_list, Edge *e);
void freeEdges(EdgeList *edge_list);
Edge *makeEdge(Point *orig, Point *dest, EdgeList *edge_list);
void destroyEdge(Edge *e, EdgeList *edge_list);
void weld(Edge *in, Edge *out);
Edge *bridge(Edge *in, Edge *out, EdgeList *edge_list);
void freeEdges(EdgeList *edge_list);

/* Sorting methods
 */
void quicksort(Point *point_list[], int l, int r);
void quickselect(Point *point_list[], size_t l, size_t r, size_t k, int (*lessThan)(Point *, Point *));
size_t partition(Point *point_list[], size_t l, size_t r, size_t k, int (*lessThan)(Point *, Point *));
void insertion_sort(Point *point_list[], size_t num_points);

/* Non-robust geometric predicates
 */
VALUE orientation(Point *a, Point *b, Point *c);
VALUE inCircle(Point *a, Point *b, Point *c, Point *d);

#endif
