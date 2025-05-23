// @HEADER
// *****************************************************************************
//        MueLu: A package for multigrid based preconditioning
//
// Copyright 2012 NTESS and the MueLu contributors.
// SPDX-License-Identifier: BSD-3-Clause
// *****************************************************************************
// @HEADER

#include <Teuchos_UnitTestHarness.hpp>
#include <Teuchos_XMLParameterListHelpers.hpp>

#include "MueLu_TestHelpers.hpp"
#include "MueLu_Version.hpp"

#include <Xpetra_MultiVectorFactory.hpp>
#include <Xpetra_MapFactory.hpp>

#include "MueLu_FactoryManagerBase.hpp"
#include "MueLu_Hierarchy.hpp"
#include "MueLu_PFactory.hpp"
#include "MueLu_SaPFactory.hpp"
#include "MueLu_TransPFactory.hpp"
#include "MueLu_RAPFactory.hpp"
#include "MueLu_AmesosSmoother.hpp"
#include "MueLu_TrilinosSmoother.hpp"
#include "MueLu_SmootherFactory.hpp"
#include "MueLu_TentativePFactory.hpp"
#include "MueLu_AmesosSmoother.hpp"
#include "MueLu_Utilities.hpp"

#include "MueLu_CreateTpetraPreconditioner.hpp"
#include "MueLu_TpetraOperator.hpp"
#ifdef HAVE_MUELU_EPETRA
#include "MueLu_CreateEpetraPreconditioner.hpp"
#include "MueLu_EpetraOperator.hpp"
#endif

// #endif

namespace MueLuTests {

TEUCHOS_UNIT_TEST_TEMPLATE_4_DECL(PetraOperator, CreatePreconditioner, Scalar, LocalOrdinal, GlobalOrdinal, Node) {
#include "MueLu_UseShortNames.hpp"
  MUELU_TESTING_SET_OSTREAM;
  MUELU_TESTING_LIMIT_SCOPE(Scalar, GlobalOrdinal, Node);

  out << "version: " << MueLu::Version() << std::endl;

  using Teuchos::RCP;
  typedef MueLu::Utilities<SC, LO, GO, NO> Utils;
  typedef typename Teuchos::ScalarTraits<SC>::magnitudeType real_type;
  typedef Xpetra::MultiVector<real_type, LO, GO, NO> RealValuedMultiVector;

  Xpetra::UnderlyingLib lib           = TestHelpers::Parameters::getLib();
  RCP<const Teuchos::Comm<int> > comm = TestHelpers::Parameters::getDefaultComm();

  std::string xmlFileName = "test.xml";

  if (lib == Xpetra::UseTpetra) {
#if defined(HAVE_MUELU_TPETRA_INST_INT_INT)
    typedef Tpetra::CrsMatrix<SC, LO, GO, NO> tpetra_crsmatrix_type;
    typedef Tpetra::Operator<SC, LO, GO, NO> tpetra_operator_type;
    typedef Tpetra::MultiVector<SC, LO, GO, NO> tpetra_multivector_type;
    typedef Xpetra::MultiVector<real_type, LO, GO, NO> dMultiVector;
    typedef Tpetra::MultiVector<real_type, LO, GO, NO> dtpetra_multivector_type;

    // Matrix
    GO nx              = 1000;
    RCP<Matrix> Op     = TestHelpers::TestFactory<SC, LO, GO, NO>::Build1DPoisson(nx * comm->getSize(), lib);
    RCP<const Map> map = Op->getRowMap();

    // Normalized RHS
    RCP<MultiVector> RHS1 = MultiVectorFactory::Build(map, 1);
    RHS1->setSeed(846930886);
    RHS1->randomize();
    Teuchos::Array<typename Teuchos::ScalarTraits<SC>::magnitudeType> norms(1);
    RHS1->norm2(norms);
    RHS1->scale(1 / norms[0]);

    // Zero initial guess
    RCP<MultiVector> X1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());

#if defined(HAVE_MUELU_ZOLTAN) && defined(HAVE_MPI)
    Teuchos::ParameterList galeriList;
    galeriList.set("nx", nx);
    RCP<RealValuedMultiVector> coordinates = Galeri::Xpetra::Utils::CreateCartesianCoordinates<real_type, LO, GO, Map, RealValuedMultiVector>("1D", Op->getRowMap(), galeriList);
    RCP<MultiVector> nullspace             = Xpetra::MultiVectorFactory<SC, LO, GO, NO>::Build(Op->getDomainMap(), 1);
    nullspace->putScalar(Teuchos::ScalarTraits<SC>::one());

    RCP<tpetra_crsmatrix_type> tpA = toTpetra(Op);

    out << "========== Create Preconditioner from xmlFile ==========" << std::endl;
    out << "xmlFileName: " << xmlFileName << std::endl;
    RCP<MueLu::TpetraOperator<SC, LO, GO, NO> > tH = MueLu::CreateTpetraPreconditioner<SC, LO, GO, NO>(RCP<tpetra_operator_type>(tpA), xmlFileName);
    tH->apply(*(toTpetra(RHS1)), *(toTpetra(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

#endif

#else
    std::cout << "Skip PetraOperator::CreatePreconditioner: Tpetra is not available (with GO=int enabled)" << std::endl;
#endif  // #if defined(HAVE_MUELU_TPETRA_INST_INT_INT)

  } else if (lib == Xpetra::UseEpetra) {
#ifdef HAVE_MUELU_EPETRA

    // Matrix
    GO nx              = 1000;
    RCP<Matrix> Op     = TestHelpers::TestFactory<SC, LO, GO, NO>::Build1DPoisson(nx * comm->getSize(), lib);
    RCP<const Map> map = Op->getRowMap();

    // Normalized RHS
    RCP<MultiVector> RHS1 = MultiVectorFactory::Build(map, 1);
    RHS1->setSeed(846930886);
    RHS1->randomize();
    Teuchos::Array<typename Teuchos::ScalarTraits<SC>::magnitudeType> norms(1);
    RHS1->norm2(norms);
    RHS1->scale(1 / norms[0]);

    // Zero initial guess
    RCP<MultiVector> X1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());

#if defined(HAVE_MUELU_ZOLTAN) && defined(HAVE_MPI)
    Teuchos::ParameterList galeriList;
    galeriList.set("nx", nx);
    RCP<RealValuedMultiVector> coordinates = Galeri::Xpetra::Utils::CreateCartesianCoordinates<real_type, LO, GO, Map, RealValuedMultiVector>("1D", Op->getRowMap(), galeriList);
    RCP<MultiVector> nullspace             = Xpetra::MultiVectorFactory<SC, LO, GO, NO>::Build(Op->getDomainMap(), 1);
    nullspace->putScalar(Teuchos::ScalarTraits<SC>::one());

    RCP<Epetra_CrsMatrix> epA = Utils::Op2NonConstEpetraCrs(Op);

    RCP<MueLu::EpetraOperator> eH = MueLu::CreateEpetraPreconditioner(epA, xmlFileName);

    eH->Apply(*(Utils::MV2EpetraMV(RHS1)), *(Utils::MV2NonConstEpetraMV(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

    xmlFileName = "testWithRebalance.xml";

    RCP<Epetra_MultiVector> epcoordinates = MueLu::Utilities<real_type, LO, GO, NO>::MV2NonConstEpetraMV(coordinates);
    RCP<Epetra_MultiVector> epnullspace   = Utils::MV2NonConstEpetraMV(nullspace);

    Teuchos::ParameterList paramList;
    Teuchos::updateParametersFromXmlFileAndBroadcast(xmlFileName, Teuchos::Ptr<Teuchos::ParameterList>(&paramList), *map->getComm());
    Teuchos::ParameterList& userParamList = paramList.sublist("user data");
    userParamList.set<RCP<Epetra_MultiVector> >("Coordinates", epcoordinates);
    userParamList.set<RCP<Epetra_MultiVector> >("Nullspace", epnullspace);

    eH = MueLu::CreateEpetraPreconditioner(epA, paramList);

    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());
    eH->Apply(*(Utils::MV2EpetraMV(RHS1)), *(Utils::MV2NonConstEpetraMV(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

#endif

#else
    std::cout << "Skip PetraOperator::CreatePreconditioner: Epetra is not available" << std::endl;
#endif

  } else {
    TEUCHOS_TEST_FOR_EXCEPTION(true, MueLu::Exceptions::InvalidArgument, "Unknown Xpetra lib");
  }

}  // CreatePreconditioner

TEUCHOS_UNIT_TEST_TEMPLATE_4_DECL(PetraOperator, CreatePreconditioner_XMLOnList, Scalar, LocalOrdinal, GlobalOrdinal, Node) {
#include "MueLu_UseShortNames.hpp"
  MUELU_TESTING_SET_OSTREAM;
  MUELU_TESTING_LIMIT_SCOPE(Scalar, GlobalOrdinal, Node);

  out << "version: " << MueLu::Version() << std::endl;

  using Teuchos::RCP;
  typedef MueLu::Utilities<SC, LO, GO, NO> Utils;
  typedef typename Teuchos::ScalarTraits<SC>::magnitudeType real_type;
  typedef Xpetra::MultiVector<real_type, LO, GO, NO> dMultiVector;
  typedef Xpetra::MultiVector<real_type, LO, GO, NO> RealValuedMultiVector;

  Xpetra::UnderlyingLib lib           = TestHelpers::Parameters::getLib();
  RCP<const Teuchos::Comm<int> > comm = TestHelpers::Parameters::getDefaultComm();

  Teuchos::ParameterList mylist;
  mylist.set("xml parameter file", "test.xml");

  if (lib == Xpetra::UseTpetra) {
#if defined(HAVE_MUELU_TPETRA_INST_INT_INT)
    typedef Tpetra::Operator<SC, LO, GO, NO> tpetra_operator_type;

    // Matrix
    GO nx              = 1000;
    RCP<Matrix> Op     = TestHelpers::TestFactory<SC, LO, GO, NO>::Build1DPoisson(nx * comm->getSize(), lib);
    RCP<const Map> map = Op->getRowMap();

    // Normalized RHS
    RCP<MultiVector> RHS1 = MultiVectorFactory::Build(map, 1);
    RHS1->setSeed(846930886);
    RHS1->randomize();
    Teuchos::Array<typename Teuchos::ScalarTraits<SC>::magnitudeType> norms(1);
    RHS1->norm2(norms);
    RHS1->scale(1 / norms[0]);

    // Zero initial guess
    RCP<MultiVector> X1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());

#if defined(HAVE_MUELU_ZOLTAN) && defined(HAVE_MPI)
    Teuchos::ParameterList galeriList;
    galeriList.set("nx", nx);
    RCP<RealValuedMultiVector> coordinates = Galeri::Xpetra::Utils::CreateCartesianCoordinates<real_type, LO, GO, Map, RealValuedMultiVector>("1D", Op->getRowMap(), galeriList);
    RCP<MultiVector> nullspace             = Xpetra::MultiVectorFactory<SC, LO, GO, NO>::Build(Op->getDomainMap(), 1);
    nullspace->putScalar(Teuchos::ScalarTraits<SC>::one());

    RCP<Tpetra::CrsMatrix<SC, LO, GO, NO> > tpA = toTpetra(Op);

    RCP<MueLu::TpetraOperator<SC, LO, GO, NO> > tH = MueLu::CreateTpetraPreconditioner<SC, LO, GO, NO>(RCP<tpetra_operator_type>(tpA), mylist);
    tH->apply(*(toTpetra(RHS1)), *(toTpetra(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

    mylist.set("xml parameter file", "testWithRebalance.xml");

    RCP<Tpetra::MultiVector<real_type, LO, GO, NO> > tpcoordinates = toTpetra(coordinates);
    RCP<Tpetra::MultiVector<SC, LO, GO, NO> > tpnullspace          = toTpetra(nullspace);

    std::string mueluXML = mylist.get("xml parameter file", "");
    Teuchos::ParameterList mueluList;
    Teuchos::updateParametersFromXmlFileAndBroadcast(mueluXML, Teuchos::Ptr<Teuchos::ParameterList>(&mueluList), *map->getComm());
    Teuchos::ParameterList& userParamList = mueluList.sublist("user data");
    userParamList.set<RCP<Tpetra::MultiVector<real_type, LO, GO, NO> > >("Coordinates", tpcoordinates);
    userParamList.set<RCP<Tpetra::MultiVector<SC, LO, GO, NO> > >("Nullspace", tpnullspace);
    tH = MueLu::CreateTpetraPreconditioner<SC, LO, GO, NO>(RCP<tpetra_operator_type>(tpA), mueluList);
    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());
    tH->apply(*(toTpetra(RHS1)), *(toTpetra(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

#endif

#else
    std::cout << "Skip PetraOperator::CreatePreconditioner_XMLOnList: Tpetra is not available (with GO=int enabled)" << std::endl;
#endif  // #if defined(HAVE_MUELU_TPETRA_INST_INT_INT)

  } else if (lib == Xpetra::UseEpetra) {
#ifdef HAVE_MUELU_EPETRA
    // Matrix
    GO nx              = 1000;
    RCP<Matrix> Op     = TestHelpers::TestFactory<SC, LO, GO, NO>::Build1DPoisson(nx * comm->getSize(), lib);
    RCP<const Map> map = Op->getRowMap();

    // Normalized RHS
    RCP<MultiVector> RHS1 = MultiVectorFactory::Build(map, 1);
    RHS1->setSeed(846930886);
    RHS1->randomize();
    Teuchos::Array<typename Teuchos::ScalarTraits<SC>::magnitudeType> norms(1);
    RHS1->norm2(norms);
    RHS1->scale(1 / norms[0]);

    // Zero initial guess
    RCP<MultiVector> X1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());

#if defined(HAVE_MUELU_ZOLTAN) && defined(HAVE_MPI)
    Teuchos::ParameterList galeriList;
    galeriList.set("nx", nx);
    RCP<dMultiVector> coordinates = Galeri::Xpetra::Utils::CreateCartesianCoordinates<real_type, LO, GO, Map, dMultiVector>("1D", Op->getRowMap(), galeriList);
    RCP<MultiVector> nullspace    = Xpetra::MultiVectorFactory<SC, LO, GO, NO>::Build(Op->getDomainMap(), 1);
    nullspace->putScalar(Teuchos::ScalarTraits<SC>::one());

    RCP<Epetra_CrsMatrix> epA = Utils::Op2NonConstEpetraCrs(Op);

    RCP<MueLu::EpetraOperator> eH = MueLu::CreateEpetraPreconditioner(epA, mylist);

    eH->Apply(*(Utils::MV2EpetraMV(RHS1)), *(Utils::MV2NonConstEpetraMV(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

    mylist.set("xml parameter file", "testWithRebalance.xml");

    RCP<Epetra_MultiVector> epcoordinates = MueLu::Utilities<real_type, LO, GO, NO>::MV2NonConstEpetraMV(coordinates);
    RCP<Epetra_MultiVector> epnullspace   = Utils::MV2NonConstEpetraMV(nullspace);

    Teuchos::ParameterList paramList      = mylist;
    Teuchos::ParameterList& userParamList = paramList.sublist("user data");
    userParamList.set<RCP<Epetra_MultiVector> >("Coordinates", epcoordinates);
    userParamList.set<RCP<Epetra_MultiVector> >("Nullspace", epnullspace);
    eH = MueLu::CreateEpetraPreconditioner(epA, paramList);

#endif

#else
    std::cout << "Skip PetraOperator::CreatePreconditioner_XMLOnList: Epetra is not available" << std::endl;
#endif

  } else {
    TEUCHOS_TEST_FOR_EXCEPTION(true, MueLu::Exceptions::InvalidArgument, "Unknown Xpetra lib");
  }

}  // CreatePreconditioner_XMLOnList

TEUCHOS_UNIT_TEST_TEMPLATE_4_DECL(PetraOperator, CreatePreconditioner_PDESystem, Scalar, LocalOrdinal, GlobalOrdinal, Node) {
#include "MueLu_UseShortNames.hpp"
  MUELU_TESTING_SET_OSTREAM;
  MUELU_TESTING_LIMIT_SCOPE(Scalar, GlobalOrdinal, Node);

  out << "version: " << MueLu::Version() << std::endl;

  using Teuchos::RCP;
  typedef MueLu::Utilities<SC, LO, GO, NO> Utils;
  typedef typename Teuchos::ScalarTraits<Scalar>::coordinateType real_type;
  typedef Xpetra::MultiVector<real_type, LO, GO, NO> dMultiVector;

#if defined(HAVE_MUELU_ZOLTAN) && defined(HAVE_MPI)

  Xpetra::UnderlyingLib lib           = TestHelpers::Parameters::getLib();
  RCP<const Teuchos::Comm<int> > comm = TestHelpers::Parameters::getDefaultComm();

  for (int k = 0; k < 2; k++) {
    std::string xmlFileName;
    if (k == 0) xmlFileName = "testPDE.xml";
    if (k == 1) xmlFileName = "testPDE1.xml";

    if (lib == Xpetra::UseTpetra) {
#if defined(HAVE_MUELU_TPETRA_INST_INT_INT)
      typedef Tpetra::Operator<SC, LO, GO, NO> tpetra_operator_type;

      int numPDEs = 3;

      // Matrix
      GO nx              = 972;
      RCP<Matrix> Op     = TestHelpers::TestFactory<SC, LO, GO, NO>::Build1DPoisson(nx * comm->getSize(), lib);
      RCP<const Map> map = Op->getRowMap();

      Teuchos::ParameterList clist;
      clist.set("nx", (nx * comm->getSize()) / numPDEs);
      RCP<const Map> cmap           = MapFactory::Build(lib, Teuchos::as<size_t>((nx * comm->getSize()) / numPDEs), Teuchos::as<int>(0), comm);
      RCP<dMultiVector> coordinates = Galeri::Xpetra::Utils::CreateCartesianCoordinates<real_type, LO, GO, Map, dMultiVector>("1D", cmap, clist);
      RCP<MultiVector> nullspace    = Xpetra::MultiVectorFactory<SC, LO, GO, NO>::Build(Op->getDomainMap(), numPDEs);
      if (numPDEs == 1) {
        nullspace->putScalar(Teuchos::ScalarTraits<Scalar>::one());
      } else {
        for (int i = 0; i < numPDEs; i++) {
          Teuchos::ArrayRCP<Scalar> nsData = nullspace->getDataNonConst(i);
          for (int j = 0; j < nsData.size(); j++) {
            GlobalOrdinal GID = Op->getDomainMap()->getGlobalElement(j) - Op->getDomainMap()->getIndexBase();
            if ((GID - i) % numPDEs == 0)
              nsData[j] = Teuchos::ScalarTraits<Scalar>::one();
          }
        }
      }

      // Normalized RHS
      RCP<MultiVector> RHS1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
      RHS1->setSeed(846930886);
      RHS1->randomize();
      Teuchos::Array<typename Teuchos::ScalarTraits<SC>::magnitudeType> norms(1);
      RHS1->norm2(norms);
      RHS1->scale(1 / norms[0]);

      // Zero initial guess
      RCP<MultiVector> X1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
      X1->putScalar(Teuchos::ScalarTraits<SC>::zero());

      RCP<Tpetra::CrsMatrix<SC, LO, GO, NO> > tpA                    = toTpetra(Op);
      RCP<Tpetra::MultiVector<real_type, LO, GO, NO> > tpcoordinates = toTpetra(coordinates);
      RCP<Tpetra::MultiVector<SC, LO, GO, NO> > tpnullspace          = toTpetra(nullspace);

      Teuchos::ParameterList paramList;
      Teuchos::updateParametersFromXmlFileAndBroadcast(xmlFileName, Teuchos::Ptr<Teuchos::ParameterList>(&paramList), *tpA->getDomainMap()->getComm());
      Teuchos::ParameterList& userParamList = paramList.sublist("user data");
      userParamList.set<RCP<Tpetra::MultiVector<real_type, LO, GO, NO> > >("Coordinates", tpcoordinates);
      userParamList.set<RCP<Tpetra::MultiVector<SC, LO, GO, NO> > >("Nullspace", tpnullspace);
      RCP<MueLu::TpetraOperator<SC, LO, GO, NO> > tH = MueLu::CreateTpetraPreconditioner<SC, LO, GO, NO>(RCP<tpetra_operator_type>(tpA), paramList);
      tH->apply(*(toTpetra(RHS1)), *(toTpetra(X1)));
      out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;
#else
      std::cout << "Skip PetraOperator::CreatePreconditioner_PDESystem: Tpetra is not available (with GO=int enabled)" << std::endl;
#endif  // #if defined(HAVE_MUELU_TPETRA_INST_INT_INT)

    } else if (lib == Xpetra::UseEpetra) {
#ifdef HAVE_MUELU_EPETRA
      int numPDEs = 3;

      // Matrix
      GO nx              = 972;
      RCP<Matrix> Op     = TestHelpers::TestFactory<SC, LO, GO, NO>::Build1DPoisson(nx * comm->getSize(), lib);
      RCP<const Map> map = Op->getRowMap();

      Teuchos::ParameterList clist;
      clist.set("nx", (nx * comm->getSize()) / numPDEs);
      RCP<const Map> cmap           = MapFactory::Build(lib, Teuchos::as<size_t>((nx * comm->getSize()) / numPDEs), Teuchos::as<int>(0), comm);
      RCP<dMultiVector> coordinates = Galeri::Xpetra::Utils::CreateCartesianCoordinates<real_type, LO, GO, Map, dMultiVector>("1D", cmap, clist);
      RCP<MultiVector> nullspace    = Xpetra::MultiVectorFactory<SC, LO, GO, NO>::Build(Op->getDomainMap(), numPDEs);
      if (numPDEs == 1) {
        nullspace->putScalar(Teuchos::ScalarTraits<Scalar>::one());
      } else {
        for (int i = 0; i < numPDEs; i++) {
          Teuchos::ArrayRCP<Scalar> nsData = nullspace->getDataNonConst(i);
          for (int j = 0; j < nsData.size(); j++) {
            GlobalOrdinal GID = Op->getDomainMap()->getGlobalElement(j) - Op->getDomainMap()->getIndexBase();
            if ((GID - i) % numPDEs == 0)
              nsData[j] = Teuchos::ScalarTraits<Scalar>::one();
          }
        }
      }

      // Normalized RHS
      RCP<MultiVector> RHS1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
      RHS1->setSeed(846930886);
      RHS1->randomize();
      Teuchos::Array<typename Teuchos::ScalarTraits<SC>::magnitudeType> norms(1);
      RHS1->norm2(norms);
      RHS1->scale(1 / norms[0]);

      // Zero initial guess
      RCP<MultiVector> X1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
      X1->putScalar(Teuchos::ScalarTraits<SC>::zero());

      RCP<Epetra_CrsMatrix> epA             = MueLu::Utilities<SC, LO, GO, NO>::Op2NonConstEpetraCrs(Op);
      RCP<Epetra_MultiVector> epcoordinates = MueLu::Utilities<real_type, LO, GO, NO>::MV2NonConstEpetraMV(coordinates);
      RCP<Epetra_MultiVector> epnullspace   = Utils::MV2NonConstEpetraMV(nullspace);

      Teuchos::ParameterList paramList;
      Teuchos::updateParametersFromXmlFileAndBroadcast(xmlFileName,
                                                       Teuchos::Ptr<Teuchos::ParameterList>(&paramList),
                                                       *map->getComm());
      paramList.set("use kokkos refactor", false);  // Done to avoid having kokkos factories called with Epetra
      Teuchos::ParameterList& userParamList = paramList.sublist("user data");
      userParamList.set<RCP<Epetra_MultiVector> >("Coordinates", epcoordinates);
      userParamList.set<RCP<Epetra_MultiVector> >("Nullspace", epnullspace);
      RCP<MueLu::EpetraOperator> eH = MueLu::CreateEpetraPreconditioner(epA, paramList);

      eH->Apply(*(Utils::MV2EpetraMV(RHS1)), *(Utils::MV2NonConstEpetraMV(X1)));
      out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

#else
      std::cout << "Skip PetraOperator::CreatePreconditioner_PDESystem: Epetra is not available" << std::endl;
#endif
    } else {
      TEUCHOS_TEST_FOR_EXCEPTION(true, MueLu::Exceptions::InvalidArgument, "Unknown Xpetra lib");
    }
  }
#endif  // defined(HAVE_MUELU_ZOLTAN) && defined(HAVE_MPI)
}

TEUCHOS_UNIT_TEST_TEMPLATE_4_DECL(PetraOperator, ReusePreconditioner, Scalar, LocalOrdinal, GlobalOrdinal, Node) {
#include "MueLu_UseShortNames.hpp"
  MUELU_TESTING_SET_OSTREAM;
  MUELU_TESTING_LIMIT_SCOPE(Scalar, GlobalOrdinal, Node);

  out << "version: " << MueLu::Version() << std::endl;

  using Teuchos::RCP;
  typedef MueLu::Utilities<SC, LO, GO, NO> Utils;
  typedef typename Teuchos::ScalarTraits<Scalar>::magnitudeType real_type;
  typedef Xpetra::MultiVector<real_type, LO, GO, NO> dMultiVector;

  Xpetra::UnderlyingLib lib           = TestHelpers::Parameters::getLib();
  RCP<const Teuchos::Comm<int> > comm = TestHelpers::Parameters::getDefaultComm();

  std::string xmlFileName = "testReuse.xml";

  if (lib == Xpetra::UseTpetra) {
#if defined(HAVE_MUELU_TPETRA_INST_INT_INT)
    typedef Tpetra::Operator<SC, LO, GO, NO> tpetra_operator_type;

    // Matrix
    GO nx              = 1000;
    RCP<Matrix> Op     = TestHelpers::TestFactory<SC, LO, GO, NO>::Build1DPoisson(nx * comm->getSize(), lib);
    RCP<const Map> map = Op->getRowMap();

    // Normalized RHS
    RCP<MultiVector> RHS1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    RHS1->setSeed(846930886);
    RHS1->randomize();
    Teuchos::Array<typename Teuchos::ScalarTraits<SC>::magnitudeType> norms(1);
    RHS1->norm2(norms);
    RHS1->scale(1 / norms[0]);

    // Zero initial guess
    RCP<MultiVector> X1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());

    RCP<Tpetra::CrsMatrix<SC, LO, GO, NO> > tpA = toTpetra(Op);

    RCP<MueLu::TpetraOperator<SC, LO, GO, NO> > tH = MueLu::CreateTpetraPreconditioner<SC, LO, GO, NO>(RCP<tpetra_operator_type>(tpA), xmlFileName);
    tH->apply(*(toTpetra(RHS1)), *(toTpetra(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

    // Reuse preconditioner
    MueLu::ReuseTpetraPreconditioner(tpA, *tH);

    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());
    tH->apply(*(toTpetra(RHS1)), *(toTpetra(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;
#else
    std::cout << "Skip PetraOperator::ReusePreconditioner: Tpetra is not available (with GO=int enabled)" << std::endl;
#endif  // #if defined(HAVE_MUELU_TPETRA_INST_INT_INT)

  } else if (lib == Xpetra::UseEpetra) {
#ifdef HAVE_MUELU_EPETRA
    // Matrix
    GO nx              = 1000;
    RCP<Matrix> Op     = TestHelpers::TestFactory<SC, LO, GO, NO>::Build1DPoisson(nx * comm->getSize(), lib);
    RCP<const Map> map = Op->getRowMap();

    // Normalized RHS
    RCP<MultiVector> RHS1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    RHS1->setSeed(846930886);
    RHS1->randomize();
    Teuchos::Array<typename Teuchos::ScalarTraits<SC>::magnitudeType> norms(1);
    RHS1->norm2(norms);
    RHS1->scale(1 / norms[0]);

    // Zero initial guess
    RCP<MultiVector> X1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());

    RCP<Epetra_CrsMatrix> epA = MueLu::Utilities<SC, LO, GO, NO>::Op2NonConstEpetraCrs(Op);

    Teuchos::ParameterList paramList;
    Teuchos::updateParametersFromXmlFileAndBroadcast(xmlFileName,
                                                     Teuchos::Ptr<Teuchos::ParameterList>(&paramList),
                                                     *map->getComm());
    paramList.set("use kokkos refactor", false);
    RCP<MueLu::EpetraOperator> eH = MueLu::CreateEpetraPreconditioner(epA, paramList);

    eH->Apply(*(Utils::MV2EpetraMV(RHS1)), *(Utils::MV2NonConstEpetraMV(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

    // Reuse preconditioner
    MueLu::ReuseEpetraPreconditioner(epA, *eH);

    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());
    eH->Apply(*(Utils::MV2EpetraMV(RHS1)), *(Utils::MV2NonConstEpetraMV(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;
#else
    std::cout << "Skip PetraOperator::ReusePreconditioner: Epetra is not available" << std::endl;
#endif

  } else {
    TEUCHOS_TEST_FOR_EXCEPTION(true, MueLu::Exceptions::InvalidArgument, "Unknown Xpetra lib");
  }
}

TEUCHOS_UNIT_TEST_TEMPLATE_4_DECL(PetraOperator, ReusePreconditioner2, Scalar, LocalOrdinal, GlobalOrdinal, Node) {
#include "MueLu_UseShortNames.hpp"
  MUELU_TESTING_SET_OSTREAM;
  MUELU_TESTING_LIMIT_SCOPE(Scalar, GlobalOrdinal, Node);

  out << "version: " << MueLu::Version() << std::endl;

  if (Teuchos::ScalarTraits<SC>::isComplex)
    return;

  using Teuchos::null;
  using Teuchos::RCP;
  typedef MueLu::Utilities<SC, LO, GO, NO> Utils;
  typedef typename Teuchos::ScalarTraits<Scalar>::magnitudeType real_type;
  typedef Xpetra::MultiVector<real_type, LO, GO, NO> dMultiVector;

  Xpetra::UnderlyingLib lib           = TestHelpers::Parameters::getLib();
  RCP<const Teuchos::Comm<int> > comm = TestHelpers::Parameters::getDefaultComm();

  Teuchos::ParameterList params;
  params.set("aggregation: type", "uncoupled");
  params.set("aggregation: drop tol", 0.02);
  params.set("coarse: max size", Teuchos::as<int>(500));

  if (lib == Xpetra::UseTpetra) {
    typedef Tpetra::Operator<SC, LO, GO, NO> tpetra_operator_type;

    // Matrix
    std::string matrixFile("TestMatrices/fuego0.mm");
    RCP<const Map> rowmap = MapFactory::Build(lib, Teuchos::as<size_t>(1500), Teuchos::as<int>(0), comm);
    RCP<Matrix> Op        = Xpetra::IO<SC, LO, GO, Node>::Read(matrixFile, rowmap, null, null, null);
    RCP<const Map> map    = Op->getRowMap();

    // Normalized RHS
    RCP<MultiVector> RHS1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    RHS1->setSeed(846930886);
    RHS1->randomize();
    Teuchos::Array<typename Teuchos::ScalarTraits<SC>::magnitudeType> norms(1);
    RHS1->norm2(norms);
    RHS1->scale(1 / norms[0]);

    // Zero initial guess
    RCP<MultiVector> X1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());

    RCP<Tpetra::CrsMatrix<SC, LO, GO, NO> > tpA    = toTpetra(Op);
    RCP<MueLu::TpetraOperator<SC, LO, GO, NO> > tH = MueLu::CreateTpetraPreconditioner<SC, LO, GO, NO>(RCP<tpetra_operator_type>(tpA), params);
    tH->apply(*(toTpetra(RHS1)), *(toTpetra(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

    // Reuse preconditioner

    matrixFile                                   = "TestMatrices/fuego1.mm";
    RCP<Matrix> Op2                              = Xpetra::IO<SC, LO, GO, Node>::Read(matrixFile, rowmap, null, null, null);
    RCP<Tpetra::CrsMatrix<SC, LO, GO, NO> > tpA2 = toTpetra(Op2);

    MueLu::ReuseTpetraPreconditioner(tpA2, *tH);

    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());
    tH->apply(*(toTpetra(RHS1)), *(toTpetra(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;
  } else if (lib == Xpetra::UseEpetra) {
#ifdef HAVE_MUELU_EPETRA
    // Matrix
    std::string matrixFile("TestMatrices/fuego0.mm");
    RCP<const Map> rowmap = MapFactory::Build(lib, Teuchos::as<size_t>(1500), Teuchos::as<int>(0), comm);
    RCP<Matrix> Op        = Xpetra::IO<SC, LO, GO, Node>::Read(matrixFile, rowmap, null, null, null);
    RCP<const Map> map    = Op->getRowMap();

    // Normalized RHS
    RCP<MultiVector> RHS1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    RHS1->setSeed(846930886);
    RHS1->randomize();
    Teuchos::Array<typename Teuchos::ScalarTraits<SC>::magnitudeType> norms(1);
    RHS1->norm2(norms);
    RHS1->scale(1 / norms[0]);

    // Zero initial guess
    RCP<MultiVector> X1 = MultiVectorFactory::Build(Op->getRowMap(), 1);
    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());

    params.set("use kokkos refactor", false);
    RCP<Epetra_CrsMatrix> epA     = MueLu::Utilities<SC, LO, GO, NO>::Op2NonConstEpetraCrs(Op);
    RCP<MueLu::EpetraOperator> eH = MueLu::CreateEpetraPreconditioner(epA, params);

    eH->Apply(*(Utils::MV2EpetraMV(RHS1)), *(Utils::MV2NonConstEpetraMV(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;

    // Reuse preconditioner

    matrixFile      = "TestMatrices/fuego1.mm";
    RCP<Matrix> Op2 = Xpetra::IO<SC, LO, GO, Node>::Read(matrixFile, rowmap, null, null, null);
    epA             = MueLu::Utilities<SC, LO, GO, NO>::Op2NonConstEpetraCrs(Op);
    MueLu::ReuseEpetraPreconditioner(epA, *eH);

    X1->putScalar(Teuchos::ScalarTraits<SC>::zero());
    eH->Apply(*(Utils::MV2EpetraMV(RHS1)), *(Utils::MV2NonConstEpetraMV(X1)));
    out << "after apply, ||b-A*x||_2 = " << std::setiosflags(std::ios::fixed) << std::setprecision(10) << Utils::ResidualNorm(*Op, *X1, *RHS1) << std::endl;
#else
    std::cout << "Skip PetraOperator::ReusePreconditioner: Epetra is not available" << std::endl;
#endif

  } else {
    TEUCHOS_TEST_FOR_EXCEPTION(true, MueLu::Exceptions::InvalidArgument, "Unknown Xpetra lib");
  }
}

#define MUELU_ETI_GROUP(Scalar, LO, GO, Node)                                                               \
  TEUCHOS_UNIT_TEST_TEMPLATE_4_INSTANT(PetraOperator, CreatePreconditioner, Scalar, LO, GO, Node)           \
  TEUCHOS_UNIT_TEST_TEMPLATE_4_INSTANT(PetraOperator, CreatePreconditioner_XMLOnList, Scalar, LO, GO, Node) \
  TEUCHOS_UNIT_TEST_TEMPLATE_4_INSTANT(PetraOperator, CreatePreconditioner_PDESystem, Scalar, LO, GO, Node) \
  TEUCHOS_UNIT_TEST_TEMPLATE_4_INSTANT(PetraOperator, ReusePreconditioner, Scalar, LO, GO, Node)            \
  TEUCHOS_UNIT_TEST_TEMPLATE_4_INSTANT(PetraOperator, ReusePreconditioner2, Scalar, LO, GO, Node)

#include <MueLu_ETI_4arg.hpp>

}  // namespace MueLuTests

// #endif
