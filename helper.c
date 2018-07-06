#include "defs.h"
#include "helper.h"
#include <stdio.h>
#include "io.h"

/***********************************
 * SELECTION *************************
 ***********************************/

int compareXY(Point *a, Point *b)
{
	return (a->x < b->x) || ((a->x == b->x) && (a->y < b->y));
}

int compareYX(Point *a, Point *b)
{
	return (a->y < b->y) || ((a->y == b->y) && (a->x > b->x));
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


