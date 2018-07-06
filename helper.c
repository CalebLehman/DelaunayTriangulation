#include "Typedefine.h"
#include "helper.h"
#include <stdio.h>
#include "io.h"

/***********************************
 * POINTS **************************
 ***********************************/

Point *makePoint(VALUE x, VALUE y)
{
	Point *p = malloc(sizeof *p);
	if (p == NULL)
	{
		printf("Cannot allocate point\nExiting...\n");
		exit(1);
	}

	// Build point
	p->x = x;
	p->y = y;
	p->e = NULL;

	return p;
}

void destroyPoint(Point *p, EdgeList *edge_list)
{
	if (p == NULL) return;
	
	// Destroy adj edges
	while (p->e) destroyEdge(p->e, edge_list);
	
	free(p);
}

/* True iff a < b in lexicographic (x, y)
 * ordering
 */
int compareXY(Point *a, Point *b)
{
	return (a->x < b->x) || ((a->x == b->x) && (a->y < b-> y));
}

/* True iff a < b in the sense that
 * 	a is lower than b (a->y < b->y)
 * or 	a is at least more rightwards than b
 * 		(a->y == b->y and a->x > b->x) 
 */
int compareYX(Point *a, Point *b)
{
	return (a->y < b->y) || ((a->y == b->y) && (a->x > b->x));
}

void freePoints(Point *point_list[], size_t num_points, EdgeList *edge_list)
{
	for (size_t idx = 0; idx < num_points; idx++) destroyPoint(point_list[idx], edge_list);
	free(point_list);
}	

/***********************************
 * EDGES ***************************
 ***********************************/

Edge *getEdge(EdgeList *edge_list)
{
	if (edge_list->idx < 0)
	{
		printf("Out of edge memory (%d edges)\nExiting...\n", edge_list->size);
		exit(1);
	}

	Edge *e = (edge_list->unused_edges)[edge_list->idx];
	(edge_list->idx)--;
	return e;
}

/* Doesn't actually free memory from program,
 * just frees up memory to use for more edges.
 */
void freeEdge(EdgeList *edge_list, Edge *e)
{
	(edge_list->idx)++;
	(edge_list->unused_edges)[edge_list->idx] = e;
}

void freeEdges(EdgeList *edge_list)
{
	free(edge_list->edges);
	free(edge_list->unused_edges);
}

/* Allocate and initialize edge from
 * orig to dest.
 * Note that edge is 'disconnected' from
 * other edges initially (only knows of itself
 * and its twin)
 */
Edge *makeEdge(Point *orig, Point *dest, EdgeList *edge_list)
{
	Edge *e = getEdge(edge_list);
	Edge *et = getEdge(edge_list);
	
	// Initialize edge
	e->orig = orig;
	e->oprev = e->dnext = et;
	et->orig = dest;
	et->oprev = et->dnext = e;
	e->twin = et;
	et->twin = e;

	// Check if this is the
	// first edge on a or b
	if (!orig->e) orig->e = e;
	if (!dest->e) dest->e = et;

	// Log edge creation
/*
	printf("Added edge: ");
	showEdge(e);
	printf("\n");
*/

	return e;
}		

/* Deallocate and remove edge and its
 * twin from graph
 */
void destroyEdge(Edge *e, EdgeList *edge_list)
{
	if (e == NULL) return;
	Edge *et = e->twin;

/*
	printf("Destroyed edge: ");
	showEdge(e);
	printf("\n");
*/

	Point *orig = e->orig;
	Point *dest = et->orig;
	
	// Update edge adj to orig/dest if necessary
	
	if (orig->e == e) orig->e = (et->dnext == e) ? NULL : et->dnext;
	if (dest->e == et) dest->e = (e->dnext == et) ? NULL : e->dnext;

	// Update next/prev edges
	e->oprev->dnext = et->dnext;
	e->dnext->oprev = et->oprev;
	et->oprev->dnext = e->dnext;
	et->dnext->oprev = e->oprev;

	freeEdge(edge_list, e);
	freeEdge(edge_list, et);
}

/**********************************
 * BUILDING ***********************
 **********************************/

/* Assumes edge in SHOULD go from ? to a
 * (but is not 'attached' to a),
 * and edge out ALREADY goes from a to ??,
 * and that out should come after in,
 * that is, sets in->dnext = out
 * Welds together, updating the rest
 * of prev/next appropriately
 */
void weld(Edge *in, Edge *out)
{
	Edge *next = out;
	Edge *prev = out->oprev;

	// Update oprev/dnext as necessary
	next->oprev = in;
	in->dnext = next;
	in->twin->oprev = prev;
	prev->dnext = in->twin;	
}

/* Make edge e to bridge edge in through edge out
 * Note that in -> e -> out must form the
 * clockwise boundary of a face.
 * That is, in->dnext = e
 * 	e->dnext = out;
 */	
Edge *bridge(Edge *in, Edge *out, EdgeList *edge_list)
{
	Point *orig = in->twin->orig;
	Point *dest = out->orig;

	// Make edge
	Edge *e = makeEdge(orig, dest, edge_list);
	Edge *et = e->twin;
	

	// Weld et to orig
	weld(et, in->dnext);

	// Weld e to dest
	weld(e, out);

	return e;
}	
	
/***********************************
 * SORTING *************************
 ***********************************/

void quicksort(Point *point_list[], int l, int r)
{
	// Base cases
	if (r - l <= 0)
	{
		return;
	}
	else if (r - l == 1)
	{
		if (compareXY(point_list[r], point_list[l]))
		{
			Point *temp = point_list[l];
			point_list[l] = point_list[r];
			point_list[r] = temp;
		}
		return;
	}

	// Quicksort	
	Point *pivot = point_list[(r + l) / 2];
	int l_idx = l - 1;
	int r_idx = r + 1;
	while (l_idx < r_idx)
	{
		do {
			l_idx++;
		} while ((l_idx <= r_idx) && compareXY(point_list[l_idx], pivot));

		do {
			r_idx--;
		} while ((l_idx <= r_idx) && compareXY(pivot, point_list[r_idx]));

		if (l_idx < r_idx)
		{
			Point *temp = point_list[l_idx];
			point_list[l_idx] = point_list[r_idx];
			point_list[r_idx] = temp;
		}
	}
	
	if (l_idx > l + 1) quicksort(point_list, l, l_idx - 1);
	if (r_idx < r - 1) quicksort(point_list, r_idx + 1, r);
}

/* Partition array of points so that kth indexed 
 * element is in 'correct' index
 */
void quickselect(Point *point_list[], size_t l, size_t r, size_t k, int (*lessThan)(Point *, Point *))
{
	while (l < r)
	{
		/*
		Point *pivot = point_list[(r + l) / 2];
		SWAP(point_list[(r + l) / 2], point_list[r], Point *);
		size_t j = l;
		for (size_t i = l; i < r; i++)
		{
			if (compareXY(point_list[i], pivot))
			{
				SWAP(point_list[i], point_list[j], Point *);
				j++;
			}
		}
		SWAP(point_list[r], point_list[j], Point *);
		*/
		size_t j = partition(point_list, l, r, (r + l) / 2, lessThan);

		if (j == k) l = r;
		else if (j > k) r = j - 1;
		else l = j + 1;
	}
}

/* Partition list using list[k] as partition value
 */
size_t partition(Point *point_list[], size_t l, size_t r, size_t k, int (*lessThan)(Point *, Point *))
{
	Point *pivot = point_list[k];
	SWAP(point_list[k], point_list[r], Point *);
	size_t j = l;
	for (size_t i = l; i < r; i++)
	{
		if ((*lessThan)(point_list[i], pivot))
		{
			SWAP(point_list[i], point_list[j], Point *);
			j++;
		}
	}
	SWAP(point_list[r], point_list[j], Point *);

	return j;
}

// TODO ROBUST PREDICATES

/* Positive if a, b, c counter-clockwise
 * Negative if a, b, c clockwise
 * Zero if a, b, c collinear
 */
VALUE orientation(Point *a, Point *b, Point *c)
{
	VALUE d_11 = a->x - c->x;
	VALUE d_21 = b->x - c->x;
	VALUE d_12 = a->y - c->y;
	VALUE d_22 = b->y - c->y;

	return d_11 * d_22 - d_12 * d_21;
} 

/* Assumes a, b, c in counter-clockwise order.
 * Then positive if d in circle,
 * 	negative if d outside circle,
 * 	zero if d on circle
 */
VALUE inCircle(Point *a, Point *b, Point *c, Point *d)
{
	VALUE d_11 = a->x - d->x;
	VALUE d_12 = a->y - d->y;
	VALUE d_13 = d_11*d_11 + d_12*d_12;
	VALUE d_21 = b->x - d->x;
	VALUE d_22 = b->y - d->y;
	VALUE d_23 = d_21*d_21 + d_22*d_22;
	VALUE d_31 = c->x - d->x;
	VALUE d_32 = c->y - d->y;
	VALUE d_33 = d_31*d_31 + d_32*d_32;

	return d_11 * (d_22*d_33 - d_32*d_23) - d_12 * (d_21*d_33 - d_31*d_23) + d_13 * (d_21*d_32 - d_31*d_22);
}
