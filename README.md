# Tetrahedral remeshing: speedup of a CGAL branch over main

`remesh_bench.cpp` times only the call to `CGAL::tetrahedral_isotropic_remeshing()`.
`compare.py` runs it for two CGAL trees on the meshes in `meshes/` and reports the speedup.

## Build, once per CGAL tree

```
cmake -S . -B build_main   -DCMAKE_BUILD_TYPE=Release -DCGAL_DIR=<cgal main checkout>
cmake -S . -B build_branch -DCMAKE_BUILD_TYPE=Release -DCGAL_DIR=<cgal branch checkout>
cmake --build build_main && cmake --build build_branch
```

## Run

```
python3 compare.py build_main build_branch                           # all meshes, all settings
python3 compare.py build_main build_branch meshes/mesh3_bear.mesh    # one mesh
```

Settings: target edge length 1.5, 1.0, 0.5 and 0.25 times the input's average edge length,
with 10, 10, 3 and 3 iterations, `smooth_constrained_edges(true)`, one thread.
Each case runs main, branch, branch, main; the times are means of the two runs.

## Inputs

`meshes/`: nine volume meshes generated with `make_mesh_3`, containing only domain cells
(subdomain index != 0; the exterior cells of the Mesh_3 triangulation were removed).
Thingi IDs refer to the [Thingi10K](https://ten-thousand-models.appspot.com/) dataset.
bunny00 at 0.25x needs several GB of memory.
