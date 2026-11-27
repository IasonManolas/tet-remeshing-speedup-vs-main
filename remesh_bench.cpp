// Times CGAL::tetrahedral_isotropic_remeshing() on a MEDIT mesh.
//
// Usage: remesh_bench <input.mesh> <edge_factor> <iterations>
//
// The target edge length is edge_factor times the input's average edge length
// (averaged over the six edges of every finite cell). Only the remeshing call
// is timed. Prints: seconds, then the number of output cells.

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Real_timer.h>
#include <CGAL/Tetrahedral_remeshing/Remeshing_cell_base_3.h>
#include <CGAL/Tetrahedral_remeshing/Remeshing_vertex_base_3.h>
#include <CGAL/tetrahedral_remeshing.h>
#include <CGAL/IO/File_medit.h>

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>

using K    = CGAL::Exact_predicates_inexact_constructions_kernel;
using Vb   = CGAL::Tetrahedral_remeshing::Remeshing_vertex_base_3<K>;
using Cb   = CGAL::Tetrahedral_remeshing::Remeshing_cell_base_3<K>;
using T3   = CGAL::Triangulation_3<K, CGAL::Triangulation_data_structure_3<Vb, Cb>>;
using C3t3 = CGAL::Mesh_complex_3_in_triangulation_3<T3, int, int>;

int main(int argc, char** argv)
{
  if(argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <input.mesh> <edge_factor> <iterations>\n";
    return EXIT_FAILURE;
  }
  const double edge_factor = std::atof(argv[2]);
  const int iterations = std::atoi(argv[3]);

  C3t3 c3t3;
  T3& tr = c3t3.triangulation();
  std::ifstream is(argv[1]);
  if(!CGAL::IO::read_MEDIT(is, tr, CGAL::parameters::allow_non_manifold(true))) {
    std::cerr << "Cannot read " << argv[1] << "\n";
    return EXIT_FAILURE;
  }

  double sum = 0.;
  std::size_t n = 0;
  for(auto c : tr.finite_cell_handles())
    for(int i = 0; i < 4; ++i)
      for(int j = i + 1; j < 4; ++j, ++n)
        sum += std::sqrt(CGAL::squared_distance(c->vertex(i)->point(), c->vertex(j)->point()));
  const double target_edge_length = edge_factor * sum / n;

  CGAL::Real_timer timer;
  timer.start();
  CGAL::tetrahedral_isotropic_remeshing(c3t3, target_edge_length,
                                        CGAL::parameters::number_of_iterations(iterations)
                                                         .smooth_constrained_edges(true));
  timer.stop();

  std::cout << timer.time() << " " << tr.number_of_finite_cells() << std::endl;
  return EXIT_SUCCESS;
}
