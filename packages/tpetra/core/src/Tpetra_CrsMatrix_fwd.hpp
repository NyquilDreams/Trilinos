// @HEADER
// *****************************************************************************
//          Tpetra: Templated Linear Algebra Services Package
//
// Copyright 2008 NTESS and the Tpetra contributors.
// SPDX-License-Identifier: BSD-3-Clause
// *****************************************************************************
// @HEADER

#ifndef TPETRA_CRSMATRIX_FWD_HPP
#define TPETRA_CRSMATRIX_FWD_HPP

#include "Tpetra_Details_DefaultTypes.hpp"

/// \file Tpetra_CrsMatrix_fwd.hpp
/// \brief Forward declaration of Tpetra::CrsMatrix

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Tpetra {
template<class Scalar = ::Tpetra::Details::DefaultTypes::scalar_type,
         class LocalOrdinal = ::Tpetra::Details::DefaultTypes::local_ordinal_type,
         class GlobalOrdinal = ::Tpetra::Details::DefaultTypes::global_ordinal_type,
         class Node = ::Tpetra::Details::DefaultTypes::node_type>
class CrsMatrix;
} // namespace Tpetra
#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // TPETRA_CRSMATRIX_FWD_HPP
