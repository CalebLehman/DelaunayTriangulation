#include <stdio.h>
#include "io.h"
#include "helper.h"
#include "topology.h"

/* Read points from a file
 * Format of file should be
 *  1st line - Number of points
 *  Each following line - Space-seperated
 *      coordinates for a single point
 * File should not have an empty newline at end
 */
PointList *getPoints(char *filename)
{
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL)
    {
        printf("Failed to open %s\n", filename);
        exit(1);
    }

    PointList *point_list = malloc(sizeof *point_list);

    size_t BUFF_SIZE = 512;
    char buffer[BUFF_SIZE];
    if (fgets(buffer, BUFF_SIZE, fptr))
    {
        sscanf(buffer, "%d", &(point_list->size));
    }

    point_list->idx = point_list->size;
    point_list->points = malloc((point_list->size) * sizeof *(point_list->points));
    point_list->unused_points = malloc((point_list->size) * sizeof *(point_list->unused_points));
    for (size_t i = 0; i < point_list->size; i++)
    {
        (point_list->unused_points)[i] = point_list->points + point_list->size - 1 - i;
    }

    while (fgets(buffer, BUFF_SIZE, fptr))
    {
        VALUE x, y;
        sscanf(buffer, "%ld %ld", &x, &y);

        makePoint(x, y, point_list);
    }
    fclose(fptr);
    return point_list;
}

/* Display all points on stdout, using showPoint function
 */
void showPoints(Point *point_list[], size_t num_points)
{
    printf("Total # of points: %d\n", num_points);
    for (size_t idx = 0; idx < num_points; idx++)
    {
        showPoint(point_list[idx]);
        printf("\n");
    }
}

/* Convert point to string and print to stdout
 */
void showPoint(Point *p)
{
    printf("%ld %ld", p->x, p->y);
}

/* Convert edge to string, using show point
 * and print to stdout
 */
void showEdge(Edge *e)
{
    showPoint(e->orig);
    printf(" ");
    showPoint(e->twin->orig);
}

/* Display all edges on stdout, using showEdge function
 */
void showEdges(PointList *point_list)
{
    for (size_t idx = 0; idx < point_list->size; idx++)
    {
        int unused = 0;
        for (size_t t = 0; t < point_list->idx; t++)
        {
            if ((point_list->unused_points)[t] == point_list->points + idx)
            {
                unused = 1;
                break;
            }
        }
        if (unused) continue;

        Point p = (point_list->points)[idx];
        Edge *e = p.e;
        Edge *f = p.e;

        if (f == NULL) continue;

        do {
            f = f->twin->dnext;

            if (compareXY(f->orig, f->twin->orig))
            {
                #ifdef COORD_OUTPUT
                // Show coordinates of endpoints
                showEdge(f);
                #else
                // Show index in point list of endpoints
                printf("%d %d\t", f->orig - point_list->points, f->twin->orig - point_list->points);
                #endif
                printf("\n");
            }
        } while (f != e);
    }
}
