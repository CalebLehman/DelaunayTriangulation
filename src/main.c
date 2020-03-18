#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "helper.h"
#include "delaunay.h"
#include "topology.h"
#include "io.h"

void main()
{
    char filename[] = "input.txt";
    PointList *point_list = getPoints(filename);
    size_t num_points = point_list->size;

    EdgeList *edge_list = initializeEdgeList(num_points);

    Point **point_ptr_list = malloc(num_points * sizeof *point_ptr_list);
    for (size_t t = 0; t < num_points; t++)
    {
        point_ptr_list[t] = point_list->points + t;
    }

    ExtremeEdge *ex = delaunay_horizontal(point_ptr_list, num_points, edge_list);

    showEdges(point_list);

    free(ex);

    freePoints(point_list, edge_list);
    freeEdges(edge_list);
    free(edge_list);
    free(point_list);
    free(point_ptr_list);
}
