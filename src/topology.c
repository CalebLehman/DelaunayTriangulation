#include <stdio.h>
#include <stdlib.h>
#include "topology.h"
#include "io.h"

/***********************************
 * POINTS **************************
 ***********************************/

Point *getPoint(PointList *point_list)
{
    if (point_list->idx == 0)
    {
        printf("Out of vertex memory (%zu vertices)\nExiting...\n", point_list->size);
        exit(1);
    }

    (point_list->idx)--;
    Point *p = (point_list->unused_points)[point_list->idx];
    return p;
}

Point *makePoint(VALUE x, VALUE y, PointList *point_list)
{
    Point *p = getPoint(point_list);

    // Build point
    p->x = x;
    p->y = y;
    p->e = NULL;

    return p;
}

/* Doesn't actuall free memory from program,
 * just frees up memory to use for more points
 */
void destroyPoint(Point *p, PointList *point_list, EdgeList *edge_list)
{
    if (p == NULL) return;

    // Destroy adj edges
    while (p->e) destroyEdge(p->e, edge_list);

    (point_list->unused_points)[point_list->idx] = p;
    (point_list->idx)++;
}

void freePoints(PointList *point_list)
{
    free(point_list->points);
    free(point_list->unused_points);
}

/***********************************
 * EDGES ***************************
 ***********************************/

Edge *getEdge(EdgeList *edge_list)
{
    if (edge_list->idx == 0)
    {
        printf("Out of edge memory (%zu edges)\nExiting...\n", edge_list->size);
        exit(1);
    }

    (edge_list->idx)--;
    Edge *e = (edge_list->unused_edges)[edge_list->idx];
    return e;
}

/* Doesn't actually free memory from program,
 * just frees up memory to use for more edges.
 */
void freeEdge(EdgeList *edge_list, Edge *e)
{
    (edge_list->unused_edges)[edge_list->idx] = e;
    (edge_list->idx)++;
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

    return e;
}

/* Deallocate and remove edge and its
 * twin from graph
 */
void destroyEdge(Edge *e, EdgeList *edge_list)
{
    if (e == NULL) return;
    Edge *et = e->twin;

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
 *  e->dnext = out;
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

/*********************************
 * MISC **************************
 *********************************/

/* Assumes point is included in full
 * triangulation
 */
int onConvexHull(Point *p)
{
    if (p == NULL) return 0;
    Edge *e = p->e;
    if (e == NULL) return 0;

    Edge *f = e;
    do
    {
        Point *a = f->twin->orig;
        Point *b = f->twin->dnext->twin->orig;
        Point *c = f->orig;

        if (orientation(a, b, c) <= 0) return 1;
        f = f->twin->dnext;
    } while (f != e);

    return 0;
}

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
 *  negative if d outside circle,
 *  zero if d on circle
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
