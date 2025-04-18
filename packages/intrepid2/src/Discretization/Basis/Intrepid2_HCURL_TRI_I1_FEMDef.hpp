// @HEADER
// *****************************************************************************
//                           Intrepid2 Package
//
// Copyright 2007 NTESS and the Intrepid2 contributors.
// SPDX-License-Identifier: BSD-3-Clause
// *****************************************************************************
// @HEADER

/** \file   Intrepid2_HCURL_TRI_I1_FEMDef.hpp
    \brief  Definition file for default FEM basis functions of degree 1 for H(curl) functions on Triangle cells.
    \author Created by P. Bochev and D. Ridzal and K. Peterson.
            Kokkorized by Kyungjoo Kim
*/

#ifndef __INTREPID2_HCURL_TRI_I1_FEM_DEF_HPP__
#define __INTREPID2_HCURL_TRI_I1_FEM_DEF_HPP__

namespace Intrepid2 {

  // -------------------------------------------------------------------------------------                            
  namespace Impl {                                                                                                    
                                                                                                                      
    template<EOperator opType>                                                                                        
    template<typename OutputViewType,                                                                                 
             typename inputViewType>                                                                                  
    KOKKOS_INLINE_FUNCTION                                                                                            
    void     
    Basis_HCURL_TRI_I1_FEM::Serial<opType>::
    getValues(       OutputViewType output,                                                                           
               const inputViewType input ) {
      switch (opType) {
      case OPERATOR_VALUE: {
        const auto x = input(0);
        const auto y = input(1);
      
        // output is a subview of a rank-3 array with dimensions (basisCardinality_, dim0, spaceDim), dim0 iteration of range
        output.access(0, 0) = 2.0*(1.0 - y);
        output.access(0, 1) = 2.0*x;
      
        output.access(1, 0) = -2.0*y;
        output.access(1, 1) =  2.0*x;
      
        output.access(2, 0) = -2.0*y;
        output.access(2, 1) = 2.0*(-1.0 + x);
        break;
      }
      case OPERATOR_CURL: {
        // outputValues is a subview of a rank-2 array with dimensions (basisCardinality_, dim0), dim0 iteration of range
        output.access(0) = 4.0;
        output.access(1) = 4.0;
        output.access(2) = 4.0;

        break;
      }
      default: {
        INTREPID2_TEST_FOR_ABORT( opType != OPERATOR_VALUE &&
                                  opType != OPERATOR_CURL,
                                  ">>> ERROR: (Intrepid2::Basis_HCURL_TRI_I1_FEM::Serial::getValues) operator is not supported");
      }
      }
    }

    template< typename DT, 
              typename outputValueValueType, class ...outputValueProperties,
              typename inputPointValueType,  class ...inputPointProperties>
    void 
    Basis_HCURL_TRI_I1_FEM::
    getValues( const typename DT::execution_space& space,
                     Kokkos::DynRankView<outputValueValueType,outputValueProperties...> outputValues,
               const Kokkos::DynRankView<inputPointValueType, inputPointProperties...>  inputPoints,
               const EOperator operatorType ) {
      typedef          Kokkos::DynRankView<outputValueValueType,outputValueProperties...>         outputValueViewType;
      typedef          Kokkos::DynRankView<inputPointValueType, inputPointProperties...>          inputPointViewType;
      typedef typename ExecSpace<typename inputPointViewType::execution_space,typename DT::execution_space>::ExecSpaceType ExecSpaceType;

      // Number of evaluation points = dim 0 of inputPoints
      const auto loopSize = inputPoints.extent(0);
      Kokkos::RangePolicy<ExecSpaceType,Kokkos::Schedule<Kokkos::Static> > policy(space, 0, loopSize);
  
      switch (operatorType) {
      case OPERATOR_VALUE: {
        typedef Functor<outputValueViewType,inputPointViewType,OPERATOR_VALUE> FunctorType;
        Kokkos::parallel_for( policy, FunctorType(outputValues, inputPoints) );
        break;
      } 
      case OPERATOR_CURL: {
        typedef Functor<outputValueViewType,inputPointViewType,OPERATOR_CURL> FunctorType;
        Kokkos::parallel_for( policy, FunctorType(outputValues, inputPoints) );
        break;
      } 
      case OPERATOR_DIV: {
        INTREPID2_TEST_FOR_EXCEPTION( (operatorType == OPERATOR_DIV), std::invalid_argument,
                                      ">>> ERROR (Basis_HCURL_TRI_I1_FEM): DIV is invalid operator for HCURL Basis Functions");
        break;
      } 
      case OPERATOR_GRAD: {
        INTREPID2_TEST_FOR_EXCEPTION( (operatorType == OPERATOR_GRAD), std::invalid_argument,
                                      ">>> ERROR (Basis_HCURL_TRI_I1_FEM): GRAD is invalid operator for HCURL Basis Functions");
        break;
      }
      case OPERATOR_D1:
      case OPERATOR_D2:
      case OPERATOR_D3:
      case OPERATOR_D4:
      case OPERATOR_D5:
      case OPERATOR_D6:
      case OPERATOR_D7:
      case OPERATOR_D8:
      case OPERATOR_D9:
      case OPERATOR_D10:
        INTREPID2_TEST_FOR_EXCEPTION( ( (operatorType == OPERATOR_D1)    ||
                                        (operatorType == OPERATOR_D2)    ||
                                        (operatorType == OPERATOR_D3)    ||
                                        (operatorType == OPERATOR_D4)    ||
                                        (operatorType == OPERATOR_D5)    ||
                                        (operatorType == OPERATOR_D6)    ||
                                        (operatorType == OPERATOR_D7)    ||
                                        (operatorType == OPERATOR_D8)    ||
                                        (operatorType == OPERATOR_D9)    ||
                                        (operatorType == OPERATOR_D10) ),
                                      std::invalid_argument,
                                      ">>> ERROR (Basis_HCURL_TRI_I1_FEM): Invalid operator type");
        break;

      default: {
        INTREPID2_TEST_FOR_EXCEPTION( ( (operatorType != OPERATOR_VALUE) &&
                                        (operatorType != OPERATOR_GRAD)  &&
                                        (operatorType != OPERATOR_CURL)  &&
                                        (operatorType != OPERATOR_DIV)   &&
                                        (operatorType != OPERATOR_D1)    &&
                                        (operatorType != OPERATOR_D2)    &&
                                        (operatorType != OPERATOR_D3)    &&
                                        (operatorType != OPERATOR_D4)    &&
                                        (operatorType != OPERATOR_D5)    &&
                                        (operatorType != OPERATOR_D6)    &&
                                        (operatorType != OPERATOR_D7)    &&
                                        (operatorType != OPERATOR_D8)    &&
                                        (operatorType != OPERATOR_D9)    &&
                                        (operatorType != OPERATOR_D10) ),
                                      std::invalid_argument,
                                      ">>> ERROR (Basis_HCURL_TRI_I1_FEM): Invalid operator type");
      }
      }
    }
  }

  template< typename DT, typename OT, typename PT >
  Basis_HCURL_TRI_I1_FEM<DT,OT,PT>::
  Basis_HCURL_TRI_I1_FEM() {
    const ordinal_type spaceDim = 2;
    this->basisCardinality_     = 3;
    this->basisDegree_          = 1;
    this->basisCellTopologyKey_ = shards::Triangle<3>::key;
    this->basisType_            = BASIS_FEM_DEFAULT;
    this->basisCoordinates_     = COORDINATES_CARTESIAN;
    this->functionSpace_        = FUNCTION_SPACE_HCURL;

    // initialize tags
    {
      // Basis-dependent intializations
      const ordinal_type tagSize  = 4;        // size of DoF tag
      const ordinal_type posScDim = 0;        // position in the tag, counting from 0, of the subcell dim 
      const ordinal_type posScOrd = 1;        // position in the tag, counting from 0, of the subcell ordinal
      const ordinal_type posDfOrd = 2;        // position in the tag, counting from 0, of DoF ordinal relative to the subcell
  
      // An array with local DoF tags assigned to basis functions, in the order of their local enumeration 
      ordinal_type tags[12]  = {
        1, 0, 0, 1,
        1, 1, 0, 1,
        1, 2, 0, 1 };

      // host tags
      OrdinalTypeArray1DHost tagView(&tags[0], 12);
    
      // Basis-independent function sets tag and enum data in tagToOrdinal_ and ordinalToTag_ arrays:
      this->setOrdinalTagData(this->tagToOrdinal_,
                              this->ordinalToTag_,
                              tagView,
                              this->basisCardinality_,
                              tagSize,
                              posScDim,
                              posScOrd,
                              posDfOrd);
  
    }
    // dofCoords on host and create its mirror view to device
    Kokkos::DynRankView<typename ScalarViewType::value_type,typename DT::execution_space::array_layout,Kokkos::HostSpace>
      dofCoords("dofCoordsHost", this->basisCardinality_,spaceDim);

    dofCoords(0,0) =  0.5;   dofCoords(0,1) =  0.0;
    dofCoords(1,0) =  0.5;   dofCoords(1,1) =  0.5;
    dofCoords(2,0) =  0.0;   dofCoords(2,1) =  0.5;

    this->dofCoords_ = Kokkos::create_mirror_view(typename DT::memory_space(), dofCoords);
    Kokkos::deep_copy(this->dofCoords_, dofCoords);

    // dofCoeffs on host and create its mirror view to device
    Kokkos::DynRankView<typename ScalarViewType::value_type,typename DT::execution_space::array_layout,Kokkos::HostSpace>
      dofCoeffs("dofCoeffsHost", this->basisCardinality_,spaceDim);

    dofCoeffs(0,0) =  0.5;   dofCoeffs(0,1) =  0.0;
    dofCoeffs(1,0) = -0.5;   dofCoeffs(1,1) =  0.5;
    dofCoeffs(2,0) =  0.0;   dofCoeffs(2,1) = -0.5;

    this->dofCoeffs_ = Kokkos::create_mirror_view(typename DT::memory_space(), dofCoeffs);
    Kokkos::deep_copy(this->dofCoeffs_, dofCoeffs);

  }

  template<typename DT, typename OT, typename PT>
  void 
  Basis_HCURL_TRI_I1_FEM<DT,OT,PT>::getScratchSpaceSize(       
                                    ordinal_type& perTeamSpaceSize,
                                    ordinal_type& perThreadSpaceSize,
                              const PointViewType inputPoints,
                              const EOperator operatorType) const {
    perTeamSpaceSize = 0;
    perThreadSpaceSize = 0;
  }

  template<typename DT, typename OT, typename PT>
  KOKKOS_INLINE_FUNCTION
  void 
  Basis_HCURL_TRI_I1_FEM<DT,OT,PT>::getValues(       
          OutputViewType outputValues,
      const PointViewType  inputPoints,
      const EOperator operatorType,
      const typename Kokkos::TeamPolicy<typename DT::execution_space>::member_type& team_member,
      const typename DT::execution_space::scratch_memory_space & scratchStorage, 
      const ordinal_type subcellDim,
      const ordinal_type subcellOrdinal) const {

      INTREPID2_TEST_FOR_ABORT( !((subcellDim == -1) && (subcellOrdinal == -1)),
        ">>> ERROR: (Intrepid2::Basis_HCURL_TRI_I1_FEM::getValues), The capability of selecting subsets of basis functions has not been implemented yet.");

      (void) scratchStorage; //avoid unused variable warning

      const int numPoints = inputPoints.extent(0);

      switch(operatorType) {
        case OPERATOR_VALUE:
          Kokkos::parallel_for (Kokkos::TeamThreadRange (team_member, numPoints), [=] (ordinal_type& pt) {
            auto       output = Kokkos::subview( outputValues, Kokkos::ALL(), pt, Kokkos::ALL() );
            const auto input  = Kokkos::subview( inputPoints,                 pt, Kokkos::ALL() );
            Impl::Basis_HCURL_TRI_I1_FEM::Serial<OPERATOR_VALUE>::getValues( output, input);
          });
          break;
        case OPERATOR_CURL:
          Kokkos::parallel_for (Kokkos::TeamThreadRange (team_member, numPoints), [=] (ordinal_type& pt) {
            auto       output = Kokkos::subview( outputValues, Kokkos::ALL(), pt, Kokkos::ALL() );
            const auto input  = Kokkos::subview( inputPoints,                 pt, Kokkos::ALL() );
            Impl::Basis_HCURL_TRI_I1_FEM::Serial<OPERATOR_CURL>::getValues( output, input);
          });
          break;
        default: {
          INTREPID2_TEST_FOR_ABORT( true, ">>> ERROR: (Intrepid2::Basis_HCURL_TRI_!1_FEM::getValues), Operator Type not supported.");
        }
    }
  }
  
}// namespace Intrepid2
#endif
