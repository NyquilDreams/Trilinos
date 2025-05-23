KokkosSparse::sptrsv_symbolic
#############################

Defined in header ``KokkosSparse_sptrsv.hpp``

.. code:: cppkokkos

  template <typename ExecutionSpace, typename KernelHandle, typename lno_row_view_t_,
            typename lno_nnz_view_t_>
  void sptrsv_symbolic(const ExecutionSpace &space, KernelHandle *handle,
                       lno_row_view_t_ rowmap, lno_nnz_view_t_ entries);

  template <typename KernelHandle, typename lno_row_view_t_, typename lno_nnz_view_t_>
  void sptrsv_symbolic(KernelHandle *handle, lno_row_view_t_ rowmap, lno_nnz_view_t_ entries);

  template <typename ExecutionSpace, typename KernelHandle, typename lno_row_view_t_,
            typename lno_nnz_view_t_, typename scalar_nnz_view_t_>
  void sptrsv_symbolic(ExecutionSpace &space, KernelHandle *handle, lno_row_view_t_ rowmap,
                       lno_nnz_view_t_ entries, scalar_nnz_view_t_ values);

  template <typename KernelHandle, typename lno_row_view_t_, typename lno_nnz_view_t_,
            typename scalar_nnz_view_t_>
  void sptrsv_symbolic(KernelHandle *handle, lno_row_view_t_ rowmap, lno_nnz_view_t_ entries,
                       scalar_nnz_view_t_ values);

Performs symbolic analysis required to solve a triangular linear system of equations

.. math::

   A*x=b

where :math:`A` is the input triangular matrix, :math:`b` is the input right-hand-side vector, and :math:`x` is the output solution vector.

1. Perform symbolic operations for the solution of a triangular system of linear equations.
2. Same as 1. but uses the resources of the default execution space instance associated with `KernelHandle::HandleExecSpace`.
3. Perform symbolic operations for the solution of a triangular system of linear equations, optionally attempts to use cuSPARSE when available and types are a match.
4. Same as 3. but uses the resources of the default execution space instance associated with `KernelHandle::HandleExecSpace`.

..
   .. note::

      Providing an API that accepts a `KokkosSparse::CrsMatrix` would be nice to make things simpler for users.

Parameters
==========

:space: execution space instance describing the resources available to run the kernels.

:handle: an instance of ``KokkosKernels::KokkosKernelsHandle`` from which an sptrsv_handle will be used to extract control parameters.

:rowmap: row map of the input matrix :math:`A`.

:entries: column indices of the input matrix :math:`A`.

:values: values associated with the entries of the input matrix :math:`A`.

Type Requirements
-----------------

- The types of the input parameters must be consistent with the types defined by the `KernelsHandle`:

  - ``std::is_same_v<ExecutionSpace, typename KernelHandle::HandleExecSpace> == true``
  - ``std::is_same_v<typename lno_row_view_t_::non_const_value_type, typename KernelHandle::size_type> == true``
  - ``std::is_same_v<typename lno_nnz_view_t_::non_const_value_type, typename KernelHandle::nnz_lno_t> == true``
  - ``std::is_same_<vtypename scalar_nnz_view_t_::value_type, typename KernelHandle::nnz_scalar_t> == true``

Example
=======

.. code:: cppkokkos

  #include <Kokkos_Core.hpp>
  #include <KokkosSparse_CrsMatrix.hpp>
  #include <KokkosSparse_sptrsv.hpp>
  #include <KokkosKernels_IOUtils.hpp>
  #include <KokkosKernels_SparseUtils.hpp>

  int main(int argc, char* argv[]) {
    using namespace KokkosSparse;
    using namespace KokkosSparse::Experimental;
    using namespace KokkosKernels;
    using namespace KokkosKernels::Impl;
    using namespace KokkosKernels::Experimental;

    Kokkos::initialize();
    {
      using scalar_t  = double;
      using lno_t     = int;
      using size_type = int;
      using device = typename Kokkos::DefaultExecutionSpace::device_type;

      using RowMapType  = Kokkos::View< size_type*, device >;
      using EntriesType = Kokkos::View< lno_t*, device >;
      using ValuesType  = Kokkos::View< scalar_t*, device >;

      using KernelHandle = KokkosKernels::Experimental::KokkosKernelsHandle <size_type, lno_t, scalar_t,
            typename device::execution_space, typename device::memory_space, typename device::memory_space>;

      scalar_t ZERO = scalar_t(0);
      scalar_t ONE = scalar_t(1);

      const size_type nrows = 5;
      const size_type nnz   = 10;

      // Upper triangular solve: x = U \ b
      {
        RowMapType  row_map("row_map", nrows+1);
        EntriesType entries("entries", nnz);
        ValuesType  values("values", nnz);

        auto hrow_map = Kokkos::create_mirror_view(row_map);
        auto hentries = Kokkos::create_mirror_view(entries);
        auto hvalues  = Kokkos::create_mirror_view(values);

	// Fill rowmap, entries, of Crs graph for simple example matrix, values set to ones

        //  [ [1 0 1 0 0]
        //    [0 1 0 0 1]
        //    [0 0 1 1 1]
        //    [0 0 0 1 1]
        //    [0 0 0 0 1] ];

        hrow_map(0) = 0;
        hrow_map(1) = 2;
        hrow_map(2) = 4;
        hrow_map(3) = 7;
        hrow_map(4) = 9;
        hrow_map(5) = 10;

        hentries(0) = 0;
        hentries(1) = 2;
        hentries(2) = 1;
        hentries(3) = 4;
        hentries(4) = 2;
        hentries(5) = 3;
        hentries(6) = 4;
        hentries(7) = 3;
        hentries(8) = 4;
        hentries(9) = 4;

        Kokkos::deep_copy(row_map, hrow_map);
        Kokkos::deep_copy(entries, hentries);

        Kokkos::deep_copy(values, ONE);

	// Create the x and b vectors

	// Solution to find
        ValuesType x("x", nrows);

	ValuesType b("b", nrows);
        Kokkos::deep_copy(b, ONE);

	// Create sptrsv kernel handle
        KernelHandle kh;
        bool is_lower_tri = false;
        kh.create_sptrsv_handle(SPTRSVAlgorithm::SEQLVLSCHD_TP1, nrows, is_lower_tri);

        sptrsv_symbolic(&kh, row_map, entries, values);
        Kokkos::fence();

        sptrsv_solve(&kh, row_map, entries, values, b, x);
        Kokkos::fence();
        kh.destroy_sptrsv_handle();
      }


      // Lower triangular solve: x = L \ b
      {
        RowMapType  row_map("row_map", nrows+1);
        EntriesType entries("entries", nnz);
        ValuesType  values("values", nnz);

        auto hrow_map = Kokkos::create_mirror_view(row_map);
        auto hentries = Kokkos::create_mirror_view(entries);
        auto hvalues  = Kokkos::create_mirror_view(values);

	// Fill rowmap, entries, of Crs graph for simple example matrix, values set to ones

        //  [ [1 0 0 0 0]
        //    [0 1 0 0 0]
        //    [1 0 1 0 0]
        //    [0 0 1 1 0]
        //    [1 1 1 1 1] ];

	hrow_map(0) = 0;
        hrow_map(1) = 1;
        hrow_map(2) = 2;
        hrow_map(3) = 4;
        hrow_map(4) = 6;
        hrow_map(5) = 10;

        hentries(0) = 0;
        hentries(1) = 1;
        hentries(2) = 0;
        hentries(3) = 2;
        hentries(4) = 2;
        hentries(5) = 3;
        hentries(6) = 1;
        hentries(7) = 2;
        hentries(8) = 3;
        hentries(9) = 4;

        Kokkos::deep_copy(row_map, hrow_map);
        Kokkos::deep_copy(entries, hentries);

        Kokkos::deep_copy(values, ONE);

	// Create the x and b vectors

        // Solution to find
        ValuesType x("x", nrows);

        ValuesType b("b", nrows);
        Kokkos::deep_copy(b, ONE);

        // Create sptrsv kernel handle
        KernelHandle kh;
        bool is_lower_tri = true;
        kh.create_sptrsv_handle(SPTRSVAlgorithm::SEQLVLSCHD_TP1, nrows, is_lower_tri);

        sptrsv_symbolic(&kh, row_map, entries, values);
        Kokkos::fence();

        sptrsv_solve(&kh, row_map, entries, values, b, x);
        Kokkos::fence();
        kh.destroy_sptrsv_handle();
      }
    }
    Kokkos::finalize();
  }
