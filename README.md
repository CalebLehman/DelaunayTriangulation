# Delaunay Triangulation

This repository contains an C implementation of a Delaunay triangulation
algorithm for points on a regular (integer) grid. I wrote as a base
implementation, which I then re-wrote in C++ and parallelized as part of
the [SETSM](https://github.com/setsmdeveloper/SETSM) project.

# Format

This triangulation algorithm is *edge-based*. The input is a list of
points (space-separated integer pairs) preceded by the number of points.

```
example_input.txt

4
0 0
2 0
2 1
0 3
```

The output is the list of edges making up the Delaunay triangulation
(unique for points in general position, that is, unique when no 4 points
for a cyclic quadrilateral). Edges are represented as a pair of indices
in the input point list, denoting the endpoints of each edge.

```
example_output.txt

0 1
0 2
0 3
1 2
3 2
```

Compilation options allow for the output to be reported in terms of
endpoint coordinates instead of point-list indices (see
[building](#building)).

```
example_output_coordinates.txt

0 0 2 0
0 0 2 1
0 0 0 3
2 0 2 1
0 3 2 1
```

# Building

Build with the `make` command. To have output reported with endpoint
coordinates instead of point-list indices, run `make coord-output`.

# Running

Usage is `./delaunay <input-point-list>`, where the `input-point-list`
is of the format described in the [format section](#format).
