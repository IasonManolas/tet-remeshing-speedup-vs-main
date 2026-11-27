#!/usr/bin/env python3
"""Speedup of a CGAL branch over main for tetrahedral remeshing.

Usage: compare.py <main_build_dir> <branch_build_dir> [mesh ...]

Runs every mesh at every setting in the order main, branch, branch, main and
prints the mean remeshing time of each and the speedup (main time / branch time).
"""
import math
import statistics
import subprocess
import sys
from pathlib import Path

SETTINGS = [(1.5, 10), (1.0, 10), (0.5, 3), (0.25, 3)]  # (edge factor, iterations)


def run(build, mesh, factor, iters):
    out = subprocess.run([str(Path(build) / "remesh_bench"), str(mesh), str(factor), str(iters)],
                         capture_output=True, text=True, check=True).stdout.split()
    return float(out[0]), int(out[1])


def main():
    main_build, branch_build = sys.argv[1:3]
    meshes = sys.argv[3:] or sorted(str(p) for p in (Path(__file__).parent / "meshes").glob("*.mesh"))
    speedups = []
    print(f"{'mesh':22} {'factor':>6} {'iters':>5} {'main s':>9} {'branch s':>9} {'speedup':>8} {'cells main/branch':>20}")
    for factor, iters in SETTINGS:
        for mesh in meshes:
            t = {"main": [], "branch": []}
            cells = {}
            for arm in ("main", "branch", "branch", "main"):
                s, c = run(main_build if arm == "main" else branch_build, mesh, factor, iters)
                t[arm].append(s)
                cells[arm] = c
            tm, tb = statistics.mean(t["main"]), statistics.mean(t["branch"])
            speedups.append(tm / tb)
            print(f"{Path(mesh).stem:22} {factor:6} {iters:5} {tm:9.2f} {tb:9.2f} {tm / tb:7.3f}x "
                  f"{cells['main']:>10}/{cells['branch']}", flush=True)
    geomean = math.exp(statistics.mean(math.log(x) for x in speedups))
    print(f"\ngeometric mean speedup over main: {geomean:.3f}x ({len(speedups)} cases)")


if __name__ == "__main__":
    main()
