

ASSERT_DEFINED(ML_ENABLE_AztecOO)
ASSERT_DEFINED(ML_ENABLE_Amesos)
IF(ML_ENABLE_AztecOO AND ML_ENABLE_Amesos)
  TRIBITS_ADD_EXECUTABLE(
    ml_Validate
    SOURCES cxx_main.cpp
    COMM serial mpi
    )

  ASSERT_DEFINED(Python3_EXECUTABLE)
  IF (Python3_EXECUTABLE)

    TRIBITS_ADD_ADVANCED_TEST( ValidateParameters_compareTestOutput
      COMM serial mpi
      OVERALL_NUM_MPI_PROCS 1  # Should be 1,4,9
      TEST_0 EXEC ml_Validate ARGS input1.xml
        OUTPUT_FILE ValidateParameters_compareTestOutput.out
      TEST_1 CMND ${Python3_EXECUTABLE}
        ARGS ${PROJECT_SOURCE_DIR}/commonTools/test/utilities/compareTestOutput
          evaluateCriteria baseline1.txt ValidateParameters_compareTestOutput.out
        PASS_REGULAR_EXPRESSION "Test passed."
      FAIL_FAST
      )

  ENDIF()

  TRIBITS_COPY_FILES_TO_BINARY_DIR(ValidateParamsCopyFiles
    DEST_FILES input1.xml baseline1.txt evaluateCriteria baseline1.txt
    EXEDEPS ml_Validate
    )

ENDIF()
