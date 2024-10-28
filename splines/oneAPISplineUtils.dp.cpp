
#include <iostream>

#include <sycl/ext/intel/ac_types/ac_int.hpp>
#include <sycl/ext/intel/fpga_extensions.hpp>
#include <sycl/sycl.hpp>

// Included from DirectProgramming/C++SYCL_FPGA/include/
#if not defined(IS_BSP)
using sycl::ext::intel::experimental::property::usm::buffer_location;
#endif

// Forward declare the kernel and pipe names
// (This prevents unwanted name mangling in the optimization report.)
class FeederA;
class FeederB;
class Matmul;
class Drain;
class APipe;
class BPipe;
class CPipe;
class DonePipe;

/**
 * Implementation of the matrix multiplication using multiple streaming kernels.
 * Parameterized by datatype, matrix size, and tile size. Exercises the kernels
 * by running multiple repetitions for a set of matrices.
 *
 * Function arguments:
 *  q: device queue
 *  a_matrix: input matrix pointer (given in column-major)
 *  b_matrix: input matrix pointer (given in row-major, i.e., transposed)
 *  c_matrix: output matrix pointer (will be stored in column-major)
 *  repetitions: number of repetitions of the computation to execute
 *
 */
template <typename TT,          // Datatype of the elements of the matrix
          int rows_a,           // Rows of matrix A
          int common,           // Columns of matrix A / rows of matrix B
          int cols_b,           // Columns of matrix B
          int tile_a,           // Tile size for matrix A
          int tile_b,           // Tile size for matrix B
          int num_matrices>     // Number of pairs of matrices to multiply
void MatmulImpl(sycl::queue &q,            // Device queue
                std::vector<TT> &a_matrix, // Input matrix A
                std::vector<TT> &b_matrix, // Input matrix B
                std::vector<TT> &c_matrix, // Output matrix C = A * B
                int repetitions            // Number of repetitions
) {
  // Number of elements per DDR access
  // NOTE: optimized for single-precision floating-point matrices
  constexpr int kElemsPerDDRAccess = 8;

  // Matrix sizes
  constexpr int kMatsizeA = rows_a * common;
  constexpr int kMatsizeB = cols_b * common;
  constexpr int kMatsizeC = rows_a * cols_b;

  // Buffer locations for mmhost interfaces
  constexpr int kBL1 = 1;
  constexpr int kBL2 = 2;
  constexpr int kBL3 = 3;

  // Allocate FPGA DDR memory
#if defined(IS_BSP)
  TT *a = sycl::malloc_device<TT>(kMatsizeA * num_matrices, q);
  TT *b = sycl::malloc_device<TT>(kMatsizeB * num_matrices, q);
  TT *c = sycl::malloc_device<TT>(kMatsizeC * num_matrices, q);
#else
  // malloc_device are not supported when targetting an FPGA part/family
  TT *a = sycl::malloc_shared<TT>(kMatsizeA * num_matrices, q,
                                  sycl::property_list{buffer_location(kBL1)});
  TT *b = sycl::malloc_shared<TT>(kMatsizeB * num_matrices, q,
                                  sycl::property_list{buffer_location(kBL2)});
  TT *c = sycl::malloc_shared<TT>(kMatsizeC * num_matrices, q,
                                  sycl::property_list{buffer_location(kBL3)});
#endif

  // Copy matrices over
  q.memcpy(a, a_matrix.data(), kMatsizeA * num_matrices * sizeof(TT)).wait();
  q.memcpy(b, b_matrix.data(), kMatsizeB * num_matrices * sizeof(TT)).wait();




  // Free USM
  sycl::free(a, q);
  sycl::free(b, q);
  sycl::free(c, q);
}
