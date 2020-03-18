#include "defs.h"
#include "delaunay.h"
#include "topology.h"
#include "helper.h"
#include <stdio.h>
#include <stdlib.h>

EdgeList *initializeEdgeList(size_t num_points)
{
    EdgeList *edge_list = malloc(sizeof *edge_list);

    // Number of edges in triangulated graph is < 3*V
    // and we store edge and its twin -> 6*V
    edge_list->edges = malloc(6 * num_points * sizeof(*(edge_list->edges)));
    edge_list->unused_edges = malloc(6 * num_points * sizeof(*(edge_list->unused_edges)));
    for (size_t t = 0; t < 6 * num_points; t++) (edge_list->unused_edges)[t] = edge_list->edges + t;

    edge_list->idx = 6 * num_points;
    edge_list->size = 6 * num_points;

    return edge_list;
}

ExtremeEdge *delaunay2(Point *point_list[], EdgeList *edge_list)
{
    ExtremeEdge *ex = malloc(sizeof *ex);

    Point *a = point_list[0];
    Point *b = point_list[1];

    Edge *e = makeEdge(a, b, edge_list);

    if (compareXY(a, b))
    {
        ex->left_edge_ccw = e;
        ex->right_edge_cw = e->twin;
    }
    else
    {
        ex->left_edge_ccw = e->twin;
        ex->right_edge_cw = e;
    }

    if (compareYX(a, b))
    {
        ex->bottom_edge_ccw = e;
        ex->top_edge_cw = e->twin;
    }
    else
    {
        ex->bottom_edge_ccw = e->twin;
        ex->top_edge_cw = e;
    }

    return ex;
}

ExtremeEdge *delaunay3(Point *point_list[], EdgeList *edge_list)
{
    ExtremeEdge *ex = malloc(sizeof *ex);

    // Sort lexicographically by (x, y)
    quickselect(point_list, 0, 2, 1, compareXY);

    Point *a = point_list[0];
    Point *b = point_list[1];
    Point *c = point_list[2];

    // Make and weld two edges
    Edge *e1 = makeEdge(a, b, edge_list);
    Edge *e2 = makeEdge(b, c, edge_list);
    weld(e1, e2);

    // Check triangle orientation and
    // bridge over last edge, if necessary
    Edge *e3;
    VALUE signed_area = orientation(a, b, c);
    if (signed_area > 0) // a -> b -> c -> a is a counterclockwise-oriented triangle
    {
        e3 = bridge(e2, e1, edge_list);
        ex->left_edge_ccw = e1;
        ex->right_edge_cw = e2->twin;
    }
    else if (signed_area < 0) // a -> b -> c -> a is a clockwise-oriented triangle
    {
        e3 = bridge(e2, e1, edge_list);
        ex->left_edge_ccw = e3->twin;
        ex->right_edge_cw = e3;
    }
    else // p0, p1, p2 are collinear
    {
        ex->left_edge_ccw = e1;
        ex->right_edge_cw = e2->twin;
    }


    quickselect(point_list, 0, 2, 1, compareYX);
    a = point_list[0];
    b = point_list[1];
    c = point_list[2];
    e1 = a->e->twin->orig == b ? a->e : a->e->twin->dnext;
    e2 = e1->dnext;
    e3 = e2->dnext;
    signed_area = orientation(a, b, c);

    if (signed_area >= 0)
    {
        ex->bottom_edge_ccw = e1;
        ex->top_edge_cw = e2->twin;
    }
    else
    {
        ex->bottom_edge_ccw = e3->twin;
        ex->top_edge_cw = e3;
    }

    return ex;
}

ExtremeEdge *delaunay_horizontal(Point *point_list[], size_t num_points, EdgeList *edge_list)
{
    //printf("called horizontal\n");
    // BASE CASES
    if (num_points < 2)
    {
        printf("Something has gone very wrong.\nTrying to triangulate fewer than 2 points.\nExiting...\n");
        exit(1);
    }
    else if (num_points == 2)
    {
        return delaunay2(point_list, edge_list);
    }
    else if (num_points == 3)
    {
        return delaunay3(point_list, edge_list);
    }

    // RECURSION CASE
    // Divide
    size_t median = num_points / 2;
    quickselect(point_list, 0, num_points - 1, median, compareXY);

    // Recurse
    ExtremeEdge *left_ex = delaunay_vertical(point_list, median, edge_list);
    ExtremeEdge *right_ex = delaunay_vertical(point_list + median, num_points - median, edge_list);

    ExtremeEdge *ex = malloc(sizeof *ex);

    // Merge
    Edge *left_edge = left_ex->right_edge_cw;
    Edge *right_edge = right_ex->left_edge_ccw;

    // Construct lower common tangent
    // Track negatively (counter-clockwise) around left convex hull
    // Track positively (clockwise) around right convex hull
    while (1)
    {
        if (orientation(left_edge->orig, left_edge->twin->orig, right_edge->orig) > 0) left_edge = left_edge->twin->oprev->twin;
        else if (orientation(right_edge->orig, right_edge->twin->orig, left_edge->orig) < 0) right_edge = right_edge->dnext;
        else break;
    }
    Edge *lct = bridge(right_edge->oprev, left_edge->twin->dnext, edge_list);

    // For each of the convex hulls, update the extreme edges
    // of the hull + lower common tangent 
    if (left_edge->orig == left_ex->left_edge_ccw->orig) left_ex->left_edge_ccw = lct->twin;
    if (right_edge->orig == right_ex->right_edge_cw->orig) right_ex->right_edge_cw = lct;

    // Add crossing edges upwards from the lower common tangent
    Edge *new_base = lct;
    Edge *uct = NULL;
    while (new_base != NULL)
    {
        uct = new_base; 
        new_base = nextCrossEdge(uct, edge_list);
    }

    // Select extreme edges for complete convex hull
    ex->left_edge_ccw = left_ex->left_edge_ccw;
    ex->right_edge_cw = right_ex->right_edge_cw;

    Edge *temp = lct->twin;
    while (compareYX(temp->orig, temp->twin->orig)) temp = temp->oprev;
    temp = temp->dnext;
    while (compareYX(temp->twin->orig, temp->orig)) temp = temp->dnext;
    ex->bottom_edge_ccw = temp;

    temp = uct->twin;
    while (compareYX(temp->twin->orig, temp->orig)) temp = temp->twin->dnext->twin;
    temp = temp->twin->oprev->twin;
    while (compareYX(temp->orig, temp->twin->orig)) temp = temp->twin->oprev->twin;
    ex->top_edge_cw = temp;

    free(left_ex);
    free(right_ex);

    return ex;
}

ExtremeEdge *delaunay_vertical(Point *point_list[], size_t num_points, EdgeList *edge_list)
{
    //printf("called vertical\n");
    // BASE CASES
    if (num_points < 2)
    {
        printf("Something has gone very wrong.\nTrying to triangulate fewer than 2 points.\nExiting...\n");
        exit(1);
    }
    else if (num_points == 2)
    {
        return delaunay2(point_list, edge_list);
    }
    else if (num_points == 3)
    {
        return delaunay3(point_list, edge_list);
    }

    // RECURSION CASE
    // Divide
    size_t median = num_points / 2;
    quickselect(point_list, 0, num_points - 1, median, compareYX);

    // Recurse
    ExtremeEdge *bottom_ex = delaunay_horizontal(point_list, median, edge_list);
    ExtremeEdge *top_ex = delaunay_horizontal(point_list + median, num_points - median, edge_list);
    
    ExtremeEdge *ex = malloc(sizeof *ex);

    // Merge
    Edge *bottom_edge = bottom_ex->top_edge_cw;
    Edge *top_edge = top_ex->bottom_edge_ccw;

    // Construct right common tangent
    // Track negatively (counter-clockwise) around left convex hull
    // Track positively (clockwise) around right convex hull
    while (1)
    {
        if (orientation(bottom_edge->orig, bottom_edge->twin->orig, top_edge->orig) > 0) bottom_edge = bottom_edge->twin->oprev->twin;
        else if (orientation(top_edge->orig, top_edge->twin->orig, bottom_edge->orig) < 0) top_edge = top_edge->dnext;
        else break;
    }
    Edge *rct = bridge(top_edge->oprev, bottom_edge->twin->dnext, edge_list);

    // For each of the convex hulls, update the extreme edges
    // of the hull + lower common tangent 
    if (bottom_edge->orig == bottom_ex->bottom_edge_ccw->orig) bottom_ex->bottom_edge_ccw = rct->twin;
    if (top_edge->orig == top_ex->top_edge_cw->orig) top_ex->top_edge_cw = rct;

    // Add crossing edges upwards from the lower common tangent
    Edge *new_base = rct;
    Edge *lct = NULL;
    while (new_base != NULL)
    {
        lct = new_base;
        new_base = nextCrossEdge(lct, edge_list);
    }

    // Select extreme edges for complete convex hull
    ex->bottom_edge_ccw = bottom_ex->bottom_edge_ccw;
    ex->top_edge_cw = top_ex->top_edge_cw;

    Edge *temp = rct;
    while (compareXY(temp->twin->orig, temp->orig)) temp = temp->twin->dnext->twin;
    temp = temp->twin->oprev->twin;
    while (compareXY(temp->orig, temp->twin->orig)) temp = temp->twin->oprev->twin;
    ex->right_edge_cw = temp;

    temp = lct;
    while (compareXY(temp->orig, temp->twin->orig)) temp = temp->oprev;
    temp = temp->dnext;
    while (compareXY(temp->twin->orig, temp->orig)) temp = temp->dnext;
    ex->left_edge_ccw = temp;

    free(bottom_ex);
    free(top_ex);

    return ex;
}

/* Base is assumed to be an R-L edge
 * and the returned cross is either 
 *  NULL (base was the upper common tangent)
 * or   The next R-L crossing edge
 */
Edge *nextCrossEdge(Edge *base, EdgeList *edge_list)
{
    Edge *l_cand = base->dnext;
    VALUE valid_l = orientation(base->orig, l_cand->orig, l_cand->twin->orig) < 0;
    if (valid_l)
    {
        Edge *next_cand = l_cand->twin->dnext;
        while (inCircle(l_cand->orig, base->orig, l_cand->twin->orig, next_cand->twin->orig) > 0)
        {
            destroyEdge(l_cand, edge_list);
            l_cand = next_cand;
            next_cand = l_cand->twin->dnext;
        }
    }

    Edge *r_cand = base->oprev->twin;
    VALUE valid_r = orientation(base->twin->orig, base->orig, r_cand->twin->orig) > 0;
    if (valid_r)
    {
        Edge *next_cand = r_cand->oprev->twin;
        while (inCircle(base->twin->orig, base->orig, r_cand->twin->orig, next_cand->twin->orig) > 0)
        {
            destroyEdge(r_cand, edge_list);
            r_cand = next_cand;
            next_cand = r_cand->oprev->twin;
        }
    }

    if (!valid_l && !valid_r) return NULL;
    else if (!valid_l || (valid_r && (inCircle(l_cand->twin->orig, l_cand->orig, r_cand->orig, r_cand->twin->orig) > 0))) return bridge(base, r_cand->twin, edge_list)->twin;
    else return bridge(l_cand, base, edge_list)->twin;
}

void deleteAndTriangulate(Point *p, PointList *point_list, EdgeList *edge_list)
{
    if (p == NULL) return;

    Edge *e = p->e;
    if (e == NULL || onConvexHull(p))
    {
        destroyPoint(p, point_list, edge_list);
        return;
    }

    e = e->dnext;
    destroyPoint(p, point_list, edge_list);

    triangulateEmptyPolygon(e, edge_list);
    return;
}

// e is a clockwise boundary edge
void triangulateEmptyPolygon(Edge *e, EdgeList *edge_list)
{
    int n = 1;
    Edge *f = e->dnext;
    while (f != e)
    {
        n++;
        f = f->dnext;
    }

    // Base Case
    if (n <= 3)
    {
        return;
    }

    // Recurse
    Edge **edges = malloc(n * sizeof(*edges));
    for (int i = 0; i < n; i++)
    {
        edges[i] = e;
        e = e->dnext;
    }   

    // Check if tri p_1, p_0, p_i is delaunay,
    // for i = 2, ..., n-1
    int i;
    for (i = 2; i < n; i++)
    {
        int delaunay = 1;
        for (int j = 2; j < n; j++)
        {
            if ((j != i) && (inCircle(e->twin->orig, e->orig, edges[i]->orig, edges[j]->orig) > 0))
            {
                delaunay = 0;
                break;
            }
        }

        if (delaunay)
        {
            if (i == 2)
            {
                e = bridge(edges[1], edges[0], edge_list);
                triangulateEmptyPolygon(e->twin, edge_list);
            }
            else if (i == n - 1)
            {
                e = bridge(edges[0], edges[n - 1], edge_list);
                triangulateEmptyPolygon(e->twin, edge_list);
            }
            else
            {
                Edge *e1 = bridge(edges[i - 1], edges[1], edge_list);
                Edge *e2 = bridge(edges[n - 1], edges[i], edge_list);
                triangulateEmptyPolygon(e1, edge_list);
                triangulateEmptyPolygon(e2, edge_list);
            }
            break;
        }
    }
    free(edges);
}
