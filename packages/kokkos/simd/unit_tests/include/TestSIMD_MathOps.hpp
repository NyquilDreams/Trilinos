//@HEADER
// ************************************************************************
//
//                        Kokkos v. 4.0
//       Copyright (2022) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Part of Kokkos, under the Apache License v2.0 with LLVM Exceptions.
// See https://kokkos.org/LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//@HEADER

#ifndef KOKKOS_TEST_SIMD_MATH_OPS_HPP
#define KOKKOS_TEST_SIMD_MATH_OPS_HPP

#include <Kokkos_SIMD.hpp>
#include <SIMDTesting_Utilities.hpp>

template <class Abi, class Loader, class BinaryOp, class T>
void host_check_math_op_one_loader(BinaryOp binary_op, std::size_t n,
                                   T const* first_args, T const* second_args) {
  Loader loader;
  using simd_type             = Kokkos::Experimental::basic_simd<T, Abi>;
  constexpr std::size_t width = simd_type::size();
  for (std::size_t i = 0; i < n; i += width) {
    std::size_t const nremaining = n - i;
    std::size_t const nlanes     = Kokkos::min(nremaining, width);
    if ((std::is_same_v<BinaryOp, divides> ||
         std::is_same_v<BinaryOp, divides_eq>)&&nremaining < width)
      continue;
    simd_type first_arg;
    bool const loaded_first_arg =
        loader.host_load(first_args + i, nlanes, first_arg);
    simd_type second_arg;
    bool const loaded_second_arg =
        loader.host_load(second_args + i, nlanes, second_arg);
    if (!(loaded_first_arg && loaded_second_arg)) continue;

    T expected_val[width];
    for (std::size_t lane = 0; lane < width; ++lane) {
      expected_val[lane] =
          binary_op.on_host(T(first_arg[lane]), T(second_arg[lane]));
    }

    simd_type expected_result;
    expected_result.copy_from(expected_val,
                              Kokkos::Experimental::simd_flag_default);

    simd_type const computed_result = binary_op.on_host(first_arg, second_arg);
    host_check_equality(expected_result, computed_result, nlanes);
  }
}

template <class Abi, class Loader, class UnaryOp, class T>
void host_check_math_op_one_loader(UnaryOp unary_op, std::size_t n,
                                   T const* args) {
  Loader loader;
  using simd_type             = Kokkos::Experimental::basic_simd<T, Abi>;
  constexpr std::size_t width = simd_type::size();
  for (std::size_t i = 0; i < n; i += width) {
    std::size_t const nremaining = n - i;
    std::size_t const nlanes     = Kokkos::min(nremaining, width);
    simd_type arg;
    bool const loaded_arg = loader.host_load(args + i, nlanes, arg);
    if (!loaded_arg) continue;

    if constexpr (std::is_same_v<UnaryOp, cbrt_op> ||
                  std::is_same_v<UnaryOp, exp_op> ||
                  std::is_same_v<UnaryOp, log_op>)
      arg = Kokkos::abs(arg);

    typename decltype(unary_op.on_host(arg))::value_type expected_val[width];
    for (std::size_t lane = 0; lane < width; ++lane) {
      expected_val[lane] = unary_op.on_host_serial(T(arg[lane]));
    }

    decltype(unary_op.on_host(arg)) expected_result;
    expected_result.copy_from(expected_val,
                              Kokkos::Experimental::simd_flag_default);

    auto computed_result = unary_op.on_host(arg);
    host_check_equality(expected_result, computed_result, nlanes);
  }
}

template <class Abi, class Op, class... T>
inline void host_check_math_op_all_loaders(Op op, std::size_t n,
                                           T const*... args) {
  host_check_math_op_one_loader<Abi, load_element_aligned>(op, n, args...);
  host_check_math_op_one_loader<Abi, load_masked>(op, n, args...);
  host_check_math_op_one_loader<Abi, load_as_scalars>(op, n, args...);
  host_check_math_op_one_loader<Abi, load_vector_aligned>(op, n, args...);
}

template <typename Abi, typename DataType, size_t n>
inline void host_check_all_math_ops(const DataType (&first_args)[n],
                                    const DataType (&second_args)[n]) {
  host_check_math_op_all_loaders<Abi>(plus(), n, first_args, second_args);
  host_check_math_op_all_loaders<Abi>(plus_eq(), n, first_args, second_args);
  host_check_math_op_all_loaders<Abi>(minus(), n, first_args, second_args);
  host_check_math_op_all_loaders<Abi>(minus_eq(), n, first_args, second_args);
  host_check_math_op_all_loaders<Abi>(multiplies(), n, first_args, second_args);
  host_check_math_op_all_loaders<Abi>(multiplies_eq(), n, first_args,
                                      second_args);
  host_check_math_op_all_loaders<Abi>(divides(), n, first_args, second_args);
  host_check_math_op_all_loaders<Abi>(divides_eq(), n, first_args, second_args);
  host_check_math_op_all_loaders<Abi>(absolutes(), n, first_args);

  host_check_math_op_all_loaders<Abi>(floors(), n, first_args);
  host_check_math_op_all_loaders<Abi>(ceils(), n, first_args);
  host_check_math_op_all_loaders<Abi>(rounds(), n, first_args);
  host_check_math_op_all_loaders<Abi>(truncates(), n, first_args);

  host_check_math_op_all_loaders<Abi>(minimum(), n, first_args, second_args);
  host_check_math_op_all_loaders<Abi>(maximum(), n, first_args, second_args);

  // TODO: Place fallback implementations for all simd integer types
  if constexpr (std::is_floating_point_v<DataType>) {
#if defined(__INTEL_COMPILER) && \
    (defined(KOKKOS_ARCH_AVX2) || defined(KOKKOS_ARCH_AVX512XEON))
    host_check_math_op_all_loaders<Abi>(cbrt_op(), n, first_args);
    host_check_math_op_all_loaders<Abi>(exp_op(), n, first_args);
    host_check_math_op_all_loaders<Abi>(log_op(), n, first_args);
#endif
  }
}

template <typename Abi, typename DataType>
inline void host_check_abi_size() {
  using simd_type = Kokkos::Experimental::basic_simd<DataType, Abi>;
  using mask_type = typename simd_type::mask_type;
  static_assert(simd_type::size() == mask_type::size());
}

template <typename Abi, typename DataType>
inline void host_check_math_ops() {
  if constexpr (is_type_v<Kokkos::Experimental::basic_simd<DataType, Abi>>) {
    constexpr size_t alignment =
        Kokkos::Experimental::basic_simd<DataType, Abi>::size() *
        sizeof(DataType);

    host_check_abi_size<Abi, DataType>();

    if constexpr (!std::is_integral_v<DataType>) {
      alignas(alignment) DataType const first_args[] = {
          0.1, 0.4, 0.5,  0.7, 1.0, 1.5,  -2.0, 10.0,
          0.0, 1.2, -2.8, 3.0, 4.0, -0.1, 5.0,  -0.2};
      alignas(alignment) DataType const second_args[] = {
          1.0,  0.2,  1.1,  1.8, -0.1,  -3.0, -2.4, 1.0,
          13.0, -3.2, -2.1, 3.0, -15.0, -0.5, -0.2, -0.2};
      host_check_all_math_ops<Abi>(first_args, second_args);
    } else {
      if constexpr (std::is_signed_v<DataType>) {
        alignas(alignment) DataType const first_args[] = {
            1, 2, -1, 10, 0, 1, -2, 10, 0, 1, -2, -3, 7, 4, -9, -15};
        alignas(alignment) DataType const second_args[] = {
            1, 2, 1, 1, 1, -3, -2, 1, 13, -3, -2, 10, -15, 7, 2, -10};
        host_check_all_math_ops<Abi>(first_args, second_args);
      } else {
        alignas(alignment) DataType const first_args[] = {
            1, 2, 1, 10, 0, 1, 2, 10, 0, 1, 2, 11, 5, 8, 2, 14};
        alignas(alignment) DataType const second_args[] = {
            1, 2, 1, 1, 1, 3, 2, 1, 13, 3, 2, 3, 6, 20, 5, 14};
        host_check_all_math_ops<Abi>(first_args, second_args);
      }
    }
  }
}

template <typename Abi, typename... DataTypes>
inline void host_check_math_ops_all_types(
    Kokkos::Experimental::Impl::data_types<DataTypes...>) {
  (host_check_math_ops<Abi, DataTypes>(), ...);
}

template <typename... Abis>
inline void host_check_math_ops_all_abis(
    Kokkos::Experimental::Impl::abi_set<Abis...>) {
  using DataTypes = Kokkos::Experimental::Impl::data_type_set;
  (host_check_math_ops_all_types<Abis>(DataTypes()), ...);
}

template <typename Abi, typename Loader, typename BinaryOp, typename T>
KOKKOS_INLINE_FUNCTION void device_check_math_op_one_loader(
    BinaryOp binary_op, std::size_t n, T const* first_args,
    T const* second_args) {
  Loader loader;
  using simd_type             = Kokkos::Experimental::basic_simd<T, Abi>;
  constexpr std::size_t width = simd_type::size();
  for (std::size_t i = 0; i < n; i += width) {
    std::size_t const nremaining = n - i;
    std::size_t const nlanes     = Kokkos::min(nremaining, width);
    if ((std::is_same_v<BinaryOp, divides> ||
         std::is_same_v<BinaryOp, divides_eq>)&&nremaining < width)
      continue;
    simd_type first_arg;
    bool const loaded_first_arg =
        loader.device_load(first_args + i, nlanes, first_arg);
    simd_type second_arg;
    bool const loaded_second_arg =
        loader.device_load(second_args + i, nlanes, second_arg);
    if (!(loaded_first_arg && loaded_second_arg)) continue;

    simd_type expected_result(KOKKOS_LAMBDA(std::size_t lane) {
      return binary_op.on_device(first_arg[lane], second_arg[lane]);
    });

    simd_type const computed_result =
        binary_op.on_device(first_arg, second_arg);
    device_check_equality(expected_result, computed_result, nlanes);
  }
}

template <typename Abi, typename Loader, typename UnaryOp, typename T>
KOKKOS_INLINE_FUNCTION void device_check_math_op_one_loader(UnaryOp unary_op,
                                                            std::size_t n,
                                                            T const* args) {
  Loader loader;
  using simd_type             = Kokkos::Experimental::basic_simd<T, Abi>;
  constexpr std::size_t width = simd_type::size();
  for (std::size_t i = 0; i < n; i += width) {
    std::size_t const nremaining = n - i;
    std::size_t const nlanes     = Kokkos::min(nremaining, width);
    simd_type arg;
    bool const loaded_arg = loader.device_load(args + i, nlanes, arg);
    if (!loaded_arg) continue;
    auto computed_result = unary_op.on_device(arg);

    decltype(computed_result) expected_result(KOKKOS_LAMBDA(std::size_t lane) {
      return unary_op.on_device_serial(arg[lane]);
    });

    device_check_equality(expected_result, computed_result, nlanes);
  }
}

template <typename Abi, typename Op, typename... T>
KOKKOS_INLINE_FUNCTION void device_check_math_op_all_loaders(Op op,
                                                             std::size_t n,
                                                             T const*... args) {
  device_check_math_op_one_loader<Abi, load_element_aligned>(op, n, args...);
  device_check_math_op_one_loader<Abi, load_masked>(op, n, args...);
  device_check_math_op_one_loader<Abi, load_as_scalars>(op, n, args...);
  device_check_math_op_one_loader<Abi, load_vector_aligned>(op, n, args...);
}

template <typename Abi, typename DataType, size_t n>
KOKKOS_INLINE_FUNCTION void device_check_all_math_ops(
    const DataType (&first_args)[n], const DataType (&second_args)[n]) {
  device_check_math_op_all_loaders<Abi>(plus(), n, first_args, second_args);
  device_check_math_op_all_loaders<Abi>(plus_eq(), n, first_args, second_args);
  device_check_math_op_all_loaders<Abi>(minus(), n, first_args, second_args);
  device_check_math_op_all_loaders<Abi>(minus_eq(), n, first_args, second_args);
  device_check_math_op_all_loaders<Abi>(multiplies(), n, first_args,
                                        second_args);
  device_check_math_op_all_loaders<Abi>(multiplies_eq(), n, first_args,
                                        second_args);
  device_check_math_op_all_loaders<Abi>(divides(), n, first_args, second_args);
  device_check_math_op_all_loaders<Abi>(divides_eq(), n, first_args,
                                        second_args);
  device_check_math_op_all_loaders<Abi>(absolutes(), n, first_args);

  device_check_math_op_all_loaders<Abi>(floors(), n, first_args);
  device_check_math_op_all_loaders<Abi>(ceils(), n, first_args);
  device_check_math_op_all_loaders<Abi>(rounds(), n, first_args);
  device_check_math_op_all_loaders<Abi>(truncates(), n, first_args);

  device_check_math_op_all_loaders<Abi>(minimum(), n, first_args, second_args);
  device_check_math_op_all_loaders<Abi>(maximum(), n, first_args, second_args);
}

template <typename Abi, typename DataType>
KOKKOS_INLINE_FUNCTION void device_check_abi_size() {
  using simd_type = Kokkos::Experimental::basic_simd<DataType, Abi>;
  using mask_type = typename simd_type::mask_type;
  static_assert(simd_type::size() == mask_type::size());
}

template <typename Abi, typename DataType>
KOKKOS_INLINE_FUNCTION void device_check_math_ops() {
  if constexpr (is_type_v<Kokkos::Experimental::basic_simd<DataType, Abi>>) {
    device_check_abi_size<Abi, DataType>();

    if constexpr (!std::is_integral_v<DataType>) {
      DataType const first_args[]  = {0.1,  0.4,  0.5, 0.7, 1.0,  1.5,
                                      -2.0, 10.0, 0.0, 1.2, -2.8, 3.0,
                                      4.0,  -0.1, 5.0, -0.2};
      DataType const second_args[] = {1.0,   0.2,  1.1,  1.8,  -0.1, -3.0,
                                      -2.4,  1.0,  13.0, -3.2, -2.1, 3.0,
                                      -15.0, -0.5, -0.2, -0.2};
      device_check_all_math_ops<Abi>(first_args, second_args);
    } else {
      if constexpr (std::is_signed_v<DataType>) {
        DataType const first_args[]  = {1, 2, -1, 10, 0, 1, -2, 10,
                                        0, 1, -2, -3, 7, 4, -9, -15};
        DataType const second_args[] = {1,  2,  1,  1,  1,   -3, -2, 1,
                                        13, -3, -2, 10, -15, 7,  2,  -10};
        device_check_all_math_ops<Abi>(first_args, second_args);
      } else {
        DataType const first_args[]  = {1, 2, 1, 10, 0, 1, 2, 10,
                                        0, 1, 2, 11, 5, 8, 2, 14};
        DataType const second_args[] = {1,  2, 1, 1, 1, 3,  2, 1,
                                        13, 3, 2, 3, 6, 20, 5, 14};
        device_check_all_math_ops<Abi>(first_args, second_args);
      }
    }
  }
}

template <typename Abi, typename... DataTypes>
KOKKOS_INLINE_FUNCTION void device_check_math_ops_all_types(
    Kokkos::Experimental::Impl::data_types<DataTypes...>) {
  (device_check_math_ops<Abi, DataTypes>(), ...);
}

template <typename... Abis>
KOKKOS_INLINE_FUNCTION void device_check_math_ops_all_abis(
    Kokkos::Experimental::Impl::abi_set<Abis...>) {
  using DataTypes = Kokkos::Experimental::Impl::data_type_set;
  (device_check_math_ops_all_types<Abis>(DataTypes()), ...);
}

class simd_device_math_ops_functor {
 public:
  KOKKOS_INLINE_FUNCTION void operator()(int) const {
    device_check_math_ops_all_abis(
        Kokkos::Experimental::Impl::device_abi_set());
  }
};

TEST(simd, host_math_ops) {
  host_check_math_ops_all_abis(Kokkos::Experimental::Impl::host_abi_set());
}

TEST(simd, device_math_ops) {
#ifdef KOKKOS_ENABLE_OPENMPTARGET  // FIXME_OPENMPTARGET
  GTEST_SKIP()
      << "skipping because of a non-deterministic failure reporting: "
         "Failure to synchronize stream (nil): Error in "
         "cuStreamSynchronize: an illegal memory access was encountered";
#endif
#if defined(KOKKOS_ENABLE_OPENACC) && \
    defined(KOKKOS_COMPILER_CLANG)  // FIXME_CLACC
  GTEST_SKIP()
      << "skipping because of a non-deterministic failure reporting: "
         "Failure to synchronize stream (nil): Error in "
         "cuStreamSynchronize: an illegal memory access was encountered";
#endif
  Kokkos::parallel_for(1, simd_device_math_ops_functor());
}

#endif
