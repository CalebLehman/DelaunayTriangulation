#ifndef IO_H
#define IO_H

#include <stdio.h>
#include "Typedefine.h"

void getPoints(char *filename, Point **point_list[], size_t *num_points);
void showPoint(Point *p);
void showPoints(Point *point_list[], size_t num_points);
void showEdge(Edge *e);
void showEdges(Point *point_list[], size_t num_points);
void showPointEdges(Point *point_list[], size_t num_points);

#endif
