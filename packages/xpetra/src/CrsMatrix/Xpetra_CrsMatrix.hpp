// @HEADER
// *****************************************************************************
//             Xpetra: A linear algebra interface package
//
// Copyright 2012 NTESS and the Xpetra contributors.
// SPDX-License-Identifier: BSD-3-Clause
// *****************************************************************************
// @HEADER

#ifndef XPETRA_CRSMATRIX_HPP
#define XPETRA_CRSMATRIX_HPP

/* this file is automatically generated - do not edit (see script/interfaces.py) */

#include <Tpetra_KokkosCompat_DefaultNode.hpp>
#include "Xpetra_ConfigDefs.hpp"
#include "Xpetra_RowMatrix.hpp"
#include "Xpetra_DistObject.hpp"
#include "Xpetra_CrsGraph.hpp"
#include "Xpetra_Vector.hpp"

#ifdef HAVE_XPETRA_TPETRA
#include <KokkosSparse_StaticCrsGraph.hpp>
#include <KokkosSparse_CrsMatrix.hpp>
#endif

namespace Xpetra {

template <class Scalar,
          class LocalOrdinal,
          class GlobalOrdinal,
          class Node = Tpetra::KokkosClassic::DefaultNode::DefaultNodeType>
class CrsMatrix
  : public RowMatrix<Scalar, LocalOrdinal, GlobalOrdinal, Node>,
    public DistObject<char, LocalOrdinal, GlobalOrdinal, Node> {
 public:
  typedef Scalar scalar_type;
  typedef LocalOrdinal local_ordinal_type;
  typedef GlobalOrdinal global_ordinal_type;
  typedef Node node_type;

  //! @name Constructor/Destructor Methods
  //@{

  //! Destructor.
  virtual ~CrsMatrix() {}

  //@}
  //! @name Insertion/Removal Methods
  //@{

  //! Insert matrix entries, using global IDs.
  virtual void
  insertGlobalValues(GlobalOrdinal globalRow,
                     const ArrayView<const GlobalOrdinal> &cols,
                     const ArrayView<const Scalar> &vals) = 0;

  //! Insert matrix entries, using local IDs.
  virtual void insertLocalValues(LocalOrdinal localRow, const ArrayView<const LocalOrdinal> &cols, const ArrayView<const Scalar> &vals) = 0;

  //! Replace matrix entries, using global IDs.
  virtual void replaceGlobalValues(GlobalOrdinal globalRow, const ArrayView<const GlobalOrdinal> &cols, const ArrayView<const Scalar> &vals) = 0;

  //! Replace matrix entries, using local IDs.
  virtual void replaceLocalValues(LocalOrdinal localRow, const ArrayView<const LocalOrdinal> &cols, const ArrayView<const Scalar> &vals) = 0;

  //! Set all matrix entries equal to scalarThis.
  virtual void setAllToScalar(const Scalar &alpha) = 0;

  //! Scale the current values of a matrix, this = alpha*this.
  virtual void scale(const Scalar &alpha) = 0;

  /*!
  \brief Allocates and returns ArrayRCPs of the Crs arrays --- This is an Xpetra-only routine.

  \param numNonZeros Number of non-zeros
  \param rowptr Pointer to row array of Crs data
  \param colind Pointer to colInd array of Crs data
  \param values Pointer to value array of Crs data

  \warning This is an expert-only routine and should not be called from user code.
  */
  virtual void allocateAllValues(size_t numNonZeros, ArrayRCP<size_t> &rowptr, ArrayRCP<LocalOrdinal> &colind, ArrayRCP<Scalar> &values) = 0;

  //! Sets the 1D pointer arrays of the graph.
  virtual void setAllValues(const ArrayRCP<size_t> &rowptr, const ArrayRCP<LocalOrdinal> &colind, const ArrayRCP<Scalar> &values) = 0;

  //! Gets the 1D pointer arrays of the graph.
  virtual void getAllValues(ArrayRCP<const size_t> &rowptr, ArrayRCP<const LocalOrdinal> &colind, ArrayRCP<const Scalar> &values) const = 0;

  //! Gets the 1D pointer arrays of the graph.
  virtual void getAllValues(ArrayRCP<Scalar> &values) = 0;

  //@}

  //! @name Transformational Methods
  //@{

  //!
  virtual void resumeFill(const RCP<ParameterList> &params = null) = 0;

  //! Signal that data entry is complete, specifying domain and range maps.
  virtual void fillComplete(const RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> > &domainMap, const RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> > &rangeMap, const RCP<ParameterList> &params = null) = 0;

  //! Signal that data entry is complete.
  virtual void fillComplete(const RCP<ParameterList> &params = null) = 0;

  //!  Replaces the current domainMap and importer with the user-specified objects.
  virtual void replaceDomainMapAndImporter(const Teuchos::RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> > &newDomainMap, Teuchos::RCP<const Import<LocalOrdinal, GlobalOrdinal, Node> > &newImporter) = 0;

  //! Expert static fill complete
  virtual void expertStaticFillComplete(const RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> > &domainMap,
                                        const RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> > &rangeMap,
                                        const RCP<const Import<LocalOrdinal, GlobalOrdinal, Node> > &importer = Teuchos::null,
                                        const RCP<const Export<LocalOrdinal, GlobalOrdinal, Node> > &exporter = Teuchos::null,
                                        const RCP<ParameterList> &params                                      = Teuchos::null) = 0;
  //@}

  //! @name Methods implementing RowMatrix
  //@{

  //! Returns the Map that describes the row distribution in this matrix.
  virtual const RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> > getRowMap() const = 0;

  //! Returns the Map that describes the column distribution in this matrix.
  virtual const RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> > getColMap() const = 0;

  //! Returns the CrsGraph associated with this matrix.
  virtual RCP<const CrsGraph<LocalOrdinal, GlobalOrdinal, Node> > getCrsGraph() const = 0;

  //! Number of global elements in the row map of this matrix.
  virtual global_size_t getGlobalNumRows() const = 0;

  //! Number of global columns in the matrix.
  virtual global_size_t getGlobalNumCols() const = 0;

  //! Returns the number of matrix rows owned on the calling node.
  virtual size_t getLocalNumRows() const = 0;

  //! Returns the global number of entries in this matrix.
  virtual global_size_t getGlobalNumEntries() const = 0;

  //! Returns the local number of entries in this matrix.
  virtual size_t getLocalNumEntries() const = 0;

  //! Returns the current number of entries on this node in the specified local row.
  virtual size_t getNumEntriesInLocalRow(LocalOrdinal localRow) const = 0;

  //! Returns the current number of entries in the specified global row.
  virtual size_t getNumEntriesInGlobalRow(GlobalOrdinal globalRow) const = 0;

  //! Returns the maximum number of entries across all rows/columns on all nodes.
  virtual size_t getGlobalMaxNumRowEntries() const = 0;

  //! Returns the maximum number of entries across all rows/columns on this node.
  virtual size_t getLocalMaxNumRowEntries() const = 0;

  //! If matrix indices are in the local range, this function returns true. Otherwise, this function returns false.
  virtual bool isLocallyIndexed() const = 0;

  //! If matrix indices are in the global range, this function returns true. Otherwise, this function returns false.
  virtual bool isGloballyIndexed() const = 0;

  //! Returns true if the matrix is in compute mode, i.e. if fillComplete() has been called.
  virtual bool isFillComplete() const = 0;

  //! Returns true if the matrix is in edit mode.
  virtual bool isFillActive() const = 0;

  //! Returns the Frobenius norm of the matrix.
  virtual typename ScalarTraits<Scalar>::magnitudeType getFrobeniusNorm() const = 0;

  //! Returns true if getLocalRowView() and getGlobalRowView() are valid for this class.
  virtual bool supportsRowViews() const = 0;

  //! Extract a const, non-persisting view of global indices in a specified row of the matrix.
  virtual void getGlobalRowView(GlobalOrdinal GlobalRow, ArrayView<const GlobalOrdinal> &indices, ArrayView<const Scalar> &values) const = 0;

  //! Extract a list of entries in a specified global row of this matrix. Put into pre-allocated storage.
  virtual void getGlobalRowCopy(GlobalOrdinal GlobalRow, const ArrayView<GlobalOrdinal> &indices, const ArrayView<Scalar> &values, size_t &numEntries) const = 0;

  //! Extract a const, non-persisting view of local indices in a specified row of the matrix.
  virtual void getLocalRowView(LocalOrdinal LocalRow, ArrayView<const LocalOrdinal> &indices, ArrayView<const Scalar> &values) const = 0;

  //! Get a copy of the diagonal entries owned by this node, with local row indices.
  virtual void getLocalDiagCopy(Vector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &diag) const = 0;

  //! Get offsets of the diagonal entries in the matrix.
  virtual void getLocalDiagOffsets(Teuchos::ArrayRCP<size_t> &offsets) const = 0;

  //! Get a copy of the diagonal entries owned by this node, with local row indices, using row offsets.
  virtual void getLocalDiagCopy(Vector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &diag, const Teuchos::ArrayView<const size_t> &offsets) const = 0;

  //! Get a copy of the diagonal entries owned by this node, with local row indices, using row offsets.
  virtual void getLocalDiagCopy(Vector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &diag, const Kokkos::View<const size_t *, typename Node::device_type, Kokkos::MemoryUnmanaged> &offsets) const = 0;

  //! Replace the diagonal entries of the matrix
  virtual void replaceDiag(const Vector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &diag) = 0;

  //! Left scale matrix using the given vector entries
  virtual void leftScale(const Vector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &x) = 0;

  //! Right scale matrix using the given vector entries
  virtual void rightScale(const Vector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &x) = 0;

  virtual void removeEmptyProcessesInPlace(const RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> > &newMap) = 0;

  //! Returns true if globalConstants have been computed; false otherwise
  virtual bool haveGlobalConstants() const = 0;

  //@}

  //! @name Methods implementing Operator
  //@{

  /*! \brief Computes the sparse matrix-multivector multiplication.
   *
   * This method computes <tt>Y := beta*Y + alpha*Op(A)*X</tt>,
   * where <tt>Op(A)</tt> is either \f$A\f$, \f$A^T\f$ (the transpose),
   * or \f$A^H\f$ (the conjugate transpose).
   *
   * @param[in] X Input vector
   * @param[in,out] Y Result vector
   * @param[in] mode Transpose mode
   * @param[in] alpha Scaling factor
   * @param[in] beta Scaling factor
   */
  virtual void apply(const MultiVector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &X, MultiVector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &Y, Teuchos::ETransp mode = Teuchos::NO_TRANS, Scalar alpha = ScalarTraits<Scalar>::one(), Scalar beta = ScalarTraits<Scalar>::zero()) const = 0;

  /*! \brief Computes the sparse matrix-multivector multiplication for region layout matrices.
   *
   * This method is for use with matrices that are in a region layout
   * with duplicated degrees of freedom on rejgion interfaces.
   * This method computes <tt>Y := beta*Y + alpha*Op(A)*X</tt>,
   * where <tt>Op(A)</tt> is either \f$A\f$, \f$A^T\f$ (the transpose),
   * or \f$A^H\f$ (the conjugate transpose).
   *
   * @param[in] X Input vector
   * @param[in,out] Y Result vector
   * @param[in] mode Transpose mode
   * @param[in] alpha Scaling factor
   * @param[in] beta Scaling factor
   * @param[in] sumInterfaceValues Whether or not to sum interface values between regions after the matvec
   * @param[in] regionInterfaceImporter Importer with region interface information
   * @param[in] regionInterfaceLIDs Array of LIDs on region interfaces in local region format
   */
  virtual void apply(const MultiVector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &X, MultiVector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &Y, Teuchos::ETransp mode, Scalar alpha, Scalar beta, bool sumInterfaceValues, const RCP<Import<LocalOrdinal, GlobalOrdinal, Node> > &regionInterfaceImporter, const Teuchos::ArrayRCP<LocalOrdinal> &regionInterfaceLIDs) const = 0;

  //! Returns the Map associated with the domain of this operator. This will be null until fillComplete() is called.
  virtual const RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> > getDomainMap() const = 0;

  //!
  virtual const RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> > getRangeMap() const = 0;

  //@}

  //! @name Overridden from Teuchos::Describable
  //@{

  //! A simple one-line description of this object.
  virtual std::string description() const = 0;

  //! Print the object with some verbosity level to an FancyOStream object.
  virtual void describe(Teuchos::FancyOStream &out, const Teuchos::EVerbosityLevel verbLevel = Teuchos::Describable::verbLevel_default) const = 0;

  //@}

  //! @name Overridden from Teuchos::LabeledObject
  //@{
  virtual void setObjectLabel(const std::string &objectLabel) = 0;
  //@}

  //! @name Xpetra-specific routines
  //@{
#ifdef HAVE_XPETRA_TPETRA
  using impl_scalar_type = typename Kokkos::ArithTraits<Scalar>::val_type;
  using execution_space  = typename node_type::device_type;

  // that is the local_graph_type in Tpetra::CrsGraph...
  using local_graph_type = KokkosSparse::StaticCrsGraph<LocalOrdinal,
                                                        Kokkos::LayoutLeft,
                                                        execution_space,
                                                        void,
                                                        size_t>;
  /// \brief The specialization of Kokkos::CrsMatrix that represents
  ///   the part of the sparse matrix on each MPI process.
  ///  The same as for Tpetra
  using local_matrix_type = KokkosSparse::CrsMatrix<impl_scalar_type, LocalOrdinal, execution_space, void,
                                                    typename local_graph_type::size_type>;

  virtual local_matrix_type getLocalMatrixDevice() const                    = 0;
  virtual typename local_matrix_type::HostMirror getLocalMatrixHost() const = 0;

  virtual void setAllValues(const typename local_matrix_type::row_map_type &ptr,
                            const typename local_graph_type::entries_type::non_const_type &ind,
                            const typename local_matrix_type::values_type &val) = 0;
#else
#ifdef __GNUC__
#warning "Xpetra Kokkos interface for CrsMatrix is enabled (HAVE_XPETRA_KOKKOS_REFACTOR) but Tpetra is disabled. The Kokkos interface needs Tpetra to be enabled, too."
#endif
#endif

  //@}

  //  Adding these functions by hand, as they're in the skip list.

  //! Returns the number of matrix columns owned on the calling node.
  virtual size_t getLocalNumCols() const = 0;

  //! Extract a list of entries in a specified local row of the matrix. Put into storage allocated by calling routine.
  virtual void getLocalRowCopy(LocalOrdinal LocalRow, const ArrayView<LocalOrdinal> &Indices, const ArrayView<Scalar> &Values, size_t &NumEntries) const = 0;

  //! Does this have an underlying matrix
  virtual bool hasMatrix() const = 0;

  //! Returns the block size of the storage mechanism, which is usually 1, except for Tpetra::BlockCrsMatrix
  virtual LocalOrdinal GetStorageBlockSize() const = 0;

  //! Compute a residual R = B - (*this) * X
  virtual void residual(const MultiVector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &X,
                        const MultiVector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &B,
                        MultiVector<Scalar, LocalOrdinal, GlobalOrdinal, Node> &R) const = 0;

};  // CrsMatrix class

}  // namespace Xpetra

#define XPETRA_CRSMATRIX_SHORT
#endif  // XPETRA_CRSMATRIX_HPP
