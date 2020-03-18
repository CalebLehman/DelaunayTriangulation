#ifndef IO_H
#define IO_H

#include "defs.h"

PointList *getPoints(char *filename);
void showPoint(Point *p);
void showPoints(Point *point_list[], size_t num_points);
void showEdge(Edge *e);
void showEdges(PointList *point_list);

#endif
