#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include "defs.h"

/* Point functions
 */

Point *getPoint(PointList *point_list);
Point *makePoint(VALUE x, VALUE y, PointList *point_list);
void destroyPoint(Point *p, PointList *point_list, EdgeList *edge_list);
void freePoints(PointList *point_list, EdgeList *edge_list);

/* Edge functions
 */

Edge *getEdge(EdgeList *edge_list);
void destroyEdge(Edge *e, EdgeList *edge_list);
void freeEdge(EdgeList *edge_list, Edge *e);
void freeEdges(EdgeList *edge_list);

Edge *makeEdge(Point *orig, Point *dest, EdgeList *edge_list);
void weld(Edge *in, Edge *out);
Edge *bridge(Edge *in, Edge *out, EdgeList *edge_list);

/* Miscellaneous
 */

int onConvexHull(Point *p);
VALUE orientation(Point *a, Point *b, Point *c);
VALUE inCircle(Point *a, Point *b, Point *c, Point *d);


#endif
