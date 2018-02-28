# This file sets up five lists:
#	MAIN_SOURCE_FILES     List of compilation units which will be included in
#	                      the library. If it isn't on this list, it won't be
#	                      part of the library. Please try to keep it sorted to
#	                      maintain sanity.
#
#	TEST_SOURCE_FILES     List of programs that will be run as unit tests.
#
#	TEST_DATA_FILES       Files from the source three that should be made
#	                      available in the corresponding location in the build
#	                      tree in order to run tests there.
#
#	EXAMPLE_SOURCE_FILES  Other programs that will be compiled as part of the
#	                      build, but which is not part of the library nor is
#	                      run as tests.
#
#	PUBLIC_HEADER_FILES   List of public header files that should be
#	                      distributed together with the library. The source
#	                      files can of course include other files than these;
#	                      you should only add to this list if the *user* of
#	                      the library needs it.

list (APPEND MAIN_SOURCE_FILES
      opm/common/data/SimulationDataContainer.cpp
      opm/common/OpmLog/CounterLog.cpp
      opm/common/OpmLog/EclipsePRTLog.cpp
      opm/common/OpmLog/LogBackend.cpp
      opm/common/OpmLog/Logger.cpp
      opm/common/OpmLog/LogUtil.cpp
      opm/common/OpmLog/OpmLog.cpp
      opm/common/OpmLog/StreamLog.cpp
      opm/common/OpmLog/TimerLog.cpp
      opm/common/utility/numeric/MonotCubicInterpolator.cpp
      opm/common/utility/parameters/Parameter.cpp
      opm/common/utility/parameters/ParameterGroup.cpp
      opm/common/utility/parameters/ParameterTools.cpp
)
if(ENABLE_ECL_INPUT)
  list(APPEND MAIN_SOURCE_FILES
    opm/json/JsonObject.cpp
    opm/eclipse/Deck/Deck.cpp
    opm/eclipse/Deck/DeckItem.cpp
    opm/eclipse/Deck/DeckKeyword.cpp
    opm/eclipse/Deck/DeckRecord.cpp
    opm/eclipse/Deck/DeckOutput.cpp
    opm/eclipse/Deck/Section.cpp
    opm/eclipse/EclipseState/AquiferCT.cpp
    opm/eclipse/EclipseState/Aquancon.cpp
    opm/eclipse/EclipseState/checkDeck.cpp
    opm/eclipse/EclipseState/Eclipse3DProperties.cpp
    opm/eclipse/EclipseState/EclipseConfig.cpp
    opm/eclipse/EclipseState/EclipseState.cpp
    opm/eclipse/EclipseState/EndpointScaling.cpp
    opm/eclipse/EclipseState/Grid/Box.cpp
    opm/eclipse/EclipseState/Grid/BoxManager.cpp
    opm/eclipse/EclipseState/Grid/EclipseGrid.cpp
    opm/eclipse/EclipseState/Grid/FaceDir.cpp
    opm/eclipse/EclipseState/Grid/FaultCollection.cpp
    opm/eclipse/EclipseState/Grid/Fault.cpp
    opm/eclipse/EclipseState/Grid/FaultFace.cpp
    opm/eclipse/EclipseState/Grid/GridDims.cpp
    opm/eclipse/EclipseState/Grid/GridProperties.cpp
    opm/eclipse/EclipseState/Grid/GridProperty.cpp
    opm/eclipse/EclipseState/Grid/MULTREGTScanner.cpp
    opm/eclipse/EclipseState/Grid/NNC.cpp
    opm/eclipse/EclipseState/Grid/PinchMode.cpp
    opm/eclipse/EclipseState/Grid/SatfuncPropertyInitializers.cpp
    opm/eclipse/EclipseState/Grid/setKeywordBox.cpp
    opm/eclipse/EclipseState/Grid/TransMult.cpp
    opm/eclipse/EclipseState/InitConfig/Equil.cpp
    opm/eclipse/EclipseState/InitConfig/InitConfig.cpp
    opm/eclipse/EclipseState/IOConfig/IOConfig.cpp
    opm/eclipse/EclipseState/IOConfig/RestartConfig.cpp
    opm/eclipse/EclipseState/Runspec.cpp
    opm/eclipse/EclipseState/Schedule/Completion.cpp
    opm/eclipse/EclipseState/Schedule/CompletionSet.cpp
    opm/eclipse/EclipseState/Schedule/Events.cpp
    opm/eclipse/EclipseState/Schedule/Group.cpp
    opm/eclipse/EclipseState/Schedule/GroupTree.cpp
    opm/eclipse/EclipseState/Schedule/MessageLimits.cpp
    opm/eclipse/EclipseState/Schedule/MSW/Compsegs.cpp
    opm/eclipse/EclipseState/Schedule/MSW/Segment.cpp
    opm/eclipse/EclipseState/Schedule/MSW/SegmentSet.cpp
    opm/eclipse/EclipseState/Schedule/MSW/updatingCompletionsWithSegments.cpp
    opm/eclipse/EclipseState/Schedule/OilVaporizationProperties.cpp
    opm/eclipse/EclipseState/Schedule/Schedule.cpp
    opm/eclipse/EclipseState/Schedule/ScheduleEnums.cpp
    opm/eclipse/EclipseState/Schedule/TimeMap.cpp
    opm/eclipse/EclipseState/Schedule/Tuning.cpp
    opm/eclipse/EclipseState/Schedule/Well.cpp
    opm/eclipse/EclipseState/Schedule/WellEconProductionLimits.cpp
    opm/eclipse/EclipseState/Schedule/WellInjectionProperties.cpp
    opm/eclipse/EclipseState/Schedule/WellPolymerProperties.cpp
    opm/eclipse/EclipseState/Schedule/WellProductionProperties.cpp
    opm/eclipse/EclipseState/SimulationConfig/SimulationConfig.cpp
    opm/eclipse/EclipseState/SimulationConfig/ThresholdPressure.cpp
    opm/eclipse/EclipseState/SummaryConfig/SummaryConfig.cpp
    opm/eclipse/EclipseState/Tables/ColumnSchema.cpp
    opm/eclipse/EclipseState/Tables/JFunc.cpp
    opm/eclipse/EclipseState/Tables/PvtxTable.cpp
    opm/eclipse/EclipseState/Tables/SimpleTable.cpp
    opm/eclipse/EclipseState/Tables/TableColumn.cpp
    opm/eclipse/EclipseState/Tables/TableContainer.cpp
    opm/eclipse/EclipseState/Tables/TableIndex.cpp
    opm/eclipse/EclipseState/Tables/TableManager.cpp
    opm/eclipse/EclipseState/Tables/TableSchema.cpp
    opm/eclipse/EclipseState/Tables/Tables.cpp
    opm/eclipse/EclipseState/Tables/VFPInjTable.cpp
    opm/eclipse/EclipseState/Tables/VFPProdTable.cpp
    opm/eclipse/Parser/MessageContainer.cpp
    opm/eclipse/Parser/ParseContext.cpp
    opm/eclipse/Parser/Parser.cpp
    opm/eclipse/Parser/ParserEnums.cpp
    opm/eclipse/Parser/ParserItem.cpp
    opm/eclipse/Parser/ParserKeyword.cpp
    opm/eclipse/Parser/ParserRecord.cpp
    opm/eclipse/RawDeck/RawKeyword.cpp
    opm/eclipse/RawDeck/RawRecord.cpp
    opm/eclipse/RawDeck/StarToken.cpp
    opm/eclipse/Units/Dimension.cpp
    opm/eclipse/Units/UnitSystem.cpp
    opm/eclipse/Utility/Functional.cpp
    opm/eclipse/Utility/Stringview.cpp
  )

  if(NOT cjson_FOUND)
    list(APPEND MAIN_SOURCE_FILES external/cjson/cJSON.c)
  endif()
endif()
if(ENABLE_ECL_OUTPUT)
  list( APPEND MAIN_SOURCE_FILES
          opm/test_util/summaryIntegrationTest.cpp
          opm/test_util/summaryRegressionTest.cpp
          opm/test_util/summaryComparator.cpp
          opm/test_util/EclFilesComparator.cpp
          opm/output/eclipse/EclipseGridInspector.cpp
          opm/output/eclipse/EclipseIO.cpp
          opm/output/eclipse/LinearisedOutputTable.cpp
          opm/output/eclipse/RestartIO.cpp
          opm/output/eclipse/Summary.cpp
          opm/output/eclipse/Tables.cpp
          opm/output/eclipse/RegionCache.cpp
          opm/output/data/Solution.cpp
      )
endif()

list (APPEND TEST_SOURCE_FILES
      tests/test_cmp.cpp
      tests/test_cubic.cpp
      tests/test_messagelimiter.cpp
      tests/test_nonuniformtablelinear.cpp
      tests/test_OpmLog.cpp
      tests/test_param.cpp
      tests/test_SimulationDataContainer.cpp
      tests/test_sparsevector.cpp
      tests/test_uniformtablelinear.cpp
)
if(ENABLE_ECL_INPUT)
  list(APPEND TEST_SOURCE_FILES
    tests/eclipse/ADDREGTests.cpp
    tests/eclipse/AquiferCTTests.cpp
    tests/eclipse/AqudimsTests.cpp
    tests/eclipse/AquanconTests.cpp
    tests/eclipse/BoxTests.cpp
    tests/eclipse/ColumnSchemaTests.cpp
    tests/eclipse/CompletionTests.cpp
    tests/eclipse/COMPSEGUnits.cpp
    tests/eclipse/CopyRegTests.cpp
    tests/eclipse/DeckTests.cpp
    tests/eclipse/DynamicStateTests.cpp
    tests/eclipse/DynamicVectorTests.cpp
    tests/eclipse/Eclipse3DPropertiesTests.cpp
    tests/eclipse/EclipseGridTests.cpp
    tests/eclipse/EqualRegTests.cpp
    tests/eclipse/EventTests.cpp
    tests/eclipse/FaceDirTests.cpp
    tests/eclipse/FaultTests.cpp
    tests/eclipse/FunctionalTests.cpp
    tests/eclipse/GeomodifierTests.cpp
    tests/eclipse/GridPropertyTests.cpp
    tests/eclipse/GroupTests.cpp
    tests/eclipse/InitConfigTest.cpp
    tests/eclipse/IOConfigTests.cpp
    tests/eclipse/MessageContainerTest.cpp
    tests/eclipse/MessageLimitTests.cpp
    tests/eclipse/MultiRegTests.cpp
    tests/eclipse/MultisegmentWellTests.cpp
    tests/eclipse/MULTREGTScannerTests.cpp
    tests/eclipse/OrderedMapTests.cpp
    tests/eclipse/ParseContextTests.cpp
    tests/eclipse/PORVTests.cpp
    tests/eclipse/RawKeywordTests.cpp
    tests/eclipse/RestartConfigTests.cpp
    tests/eclipse/RunspecTests.cpp
    tests/eclipse/SatfuncPropertyInitializersTests.cpp
    tests/eclipse/ScheduleTests.cpp
    tests/eclipse/SectionTests.cpp
    tests/eclipse/SimpleTableTests.cpp
    tests/eclipse/SimulationConfigTest.cpp
    tests/eclipse/StarTokenTests.cpp
    tests/eclipse/StringTests.cpp
    tests/eclipse/SummaryConfigTests.cpp
    tests/eclipse/TabdimsTests.cpp
    tests/eclipse/TableColumnTests.cpp
    tests/eclipse/TableContainerTests.cpp
    tests/eclipse/TableManagerTests.cpp
    tests/eclipse/TableSchemaTests.cpp
    tests/eclipse/ThresholdPressureTest.cpp
    tests/eclipse/TimeMapTest.cpp
    tests/eclipse/TransMultTests.cpp
    tests/eclipse/TuningTests.cpp
    tests/eclipse/UnitTests.cpp
    tests/eclipse/ValueTests.cpp
    tests/eclipse/WellSolventTests.cpp
    tests/eclipse/WellTests.cpp)
endif()
if(ENABLE_ECL_OUTPUT)
  list (APPEND TEST_SOURCE_FILES
          tests/test_compareSummary.cpp
          tests/test_EclFilesComparator.cpp
          tests/test_EclipseIO.cpp
          tests/test_LinearisedOutputTable.cpp
          tests/test_Restart.cpp
          tests/test_RFT.cpp
          tests/test_Summary.cpp
          tests/test_Tables.cpp
          tests/test_Wells.cpp
          tests/test_writenumwells.cpp
          tests/test_Solution.cpp
          tests/test_regionCache.cpp
      )
endif()

list (APPEND TEST_DATA_FILES
      tests/testdata.param
)
if(ENABLE_ECL_OUTPUT)
  list (APPEND TEST_DATA_FILES
          tests/FIRST_SIM.DATA
          tests/summary_deck.DATA
          tests/group_group.DATA
          tests/testblackoilstate3.DATA
          tests/testrft.DATA
          tests/table_deck.DATA
          tests/summary_deck_non_constant_porosity.DATA
          tests/SUMMARY_EFF_FAC.DATA
      )
endif()

list (APPEND EXAMPLE_SOURCE_FILES
)
if(ENABLE_ECL_INPUT)
  list (APPEND EXAMPLE_SOURCE_FILES
    examples/opmi.cpp
  )
endif()
if(ENABLE_ECL_OUTPUT)
  list(APPEND EXAMPLE_SOURCE_FILES
          test_util/compareECL.cpp
          test_util/compareSummary.cpp
      )
endif()

# programs listed here will not only be compiled, but also marked for
# installation
list (APPEND PROGRAM_SOURCE_FILES
)
if(ENABLE_ECL_INPUT)
  list (APPEND PROGRAM_SOURCE_FILES
    examples/opmi.cpp
  )
endif()
if(ENABLE_ECL_OUTPUT)
  list(APPEND PROGRAM_SOURCE_FILES
          test_util/compareECL.cpp
          test_util/compareSummary.cpp
      )
endif()

list( APPEND PUBLIC_HEADER_FILES
      opm/common/ErrorMacros.hpp
      opm/common/Exceptions.hpp
      opm/common/data/SimulationDataContainer.hpp
      opm/common/OpmLog/CounterLog.hpp
      opm/common/OpmLog/EclipsePRTLog.hpp
      opm/common/OpmLog/LogBackend.hpp
      opm/common/OpmLog/Logger.hpp
      opm/common/OpmLog/LogUtil.hpp
      opm/common/OpmLog/MessageFormatter.hpp
      opm/common/OpmLog/MessageLimiter.hpp
      opm/common/OpmLog/OpmLog.hpp
      opm/common/OpmLog/StreamLog.hpp
      opm/common/OpmLog/TimerLog.hpp
      opm/common/utility/numeric/cmp.hpp
      opm/common/utility/platform_dependent/disable_warnings.h
      opm/common/utility/platform_dependent/reenable_warnings.h
      opm/common/utility/numeric/blas_lapack.h
      opm/common/utility/numeric/buildUniformMonotoneTable.hpp
      opm/common/utility/numeric/linearInterpolation.hpp
      opm/common/utility/numeric/MonotCubicInterpolator.hpp
      opm/common/utility/numeric/NonuniformTableLinear.hpp
      opm/common/utility/numeric/RootFinders.hpp
      opm/common/utility/numeric/SparseVector.hpp
      opm/common/utility/numeric/UniformTableLinear.hpp
      opm/common/utility/parameters/ParameterGroup.hpp
      opm/common/utility/parameters/ParameterGroup_impl.hpp
      opm/common/utility/parameters/Parameter.hpp
      opm/common/utility/parameters/ParameterMapItem.hpp
      opm/common/utility/parameters/ParameterRequirement.hpp
      opm/common/utility/parameters/ParameterStrings.hpp
      opm/common/utility/parameters/ParameterTools.hpp
)
