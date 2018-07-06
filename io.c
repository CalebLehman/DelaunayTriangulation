#include "io.h"
#include "helper.h"

/* Read points from a file
 * Format of file should be
 * 	1st line - Number of points
 * 	Each following line - Space-seperated
 * 		coordinates for a single point
 * File should not have an empty newline at end
 */
void getPoints(char *filename, Point **point_list[], size_t *num_points)
{
	FILE *fptr = fopen(filename, "r");
	if (fptr == NULL)
	{
		printf("Failed to open %s\n", filename);
		exit(1);
	}

	size_t BUFF_SIZE = 512;
	char buffer[BUFF_SIZE];
	if (fgets(buffer, BUFF_SIZE, fptr))
	{
		sscanf(buffer, "%d", num_points);
	}

	*point_list = malloc((*num_points) * sizeof **point_list);
		
	size_t idx = 0;
	while (fgets(buffer, BUFF_SIZE, fptr))
	{
		VALUE x, y;
		sscanf(buffer, "%ld %ld", &x, &y);

		(*point_list)[idx] = makePoint(x, y);
		idx++;
	}
	*num_points = idx;
	fclose(fptr);
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
void showEdges(Point *point_list[], size_t num_points)
{
	for (size_t idx = 0; idx < num_points; idx++)
	{
		Point *p = point_list[idx];
		Edge *e = p->e;
		Edge *f = p->e;

		do {
			f = f->twin->dnext;

			if (compareXY(f->orig, f->twin->orig))
			{
				showEdge(f);
				printf("\n");
			}
		} while (f != e);
	}
}

/*
void showPointEdges(Point *point_list[], size_t num_points)
{
	for (size_t idx = 0; idx < num_points; idx++)
	{
		printf("Point (");
		showPoint(point_list[idx]);
		printf(") goes through <<");
		if (point_list[idx]->e) showEdge(point_list[idx]->e);
		else printf("NULL");
		printf(">>\n");
	}
}
*/
