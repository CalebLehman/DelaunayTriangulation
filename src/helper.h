#ifndef HELPER_H
#define HELPER_H

#include "defs.h"

/* Selection methods
 */
int compareXY(Point *a, Point *b);
int compareYX(Point *a, Point *b);
void quickselect(Point *point_list[], size_t l, size_t r, size_t k, int (*lessThan)(Point *, Point *));
size_t partition(Point *point_list[], size_t l, size_t r, size_t k, int (*lessThan)(Point *, Point *));

#endif
