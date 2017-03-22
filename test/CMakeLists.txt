# cmake driver file for testing

# Enable asserts
include(CetTest)
cet_enable_asserts()

# For LaserUtils test
simple_plugin(LaserUtilsTest "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        lardataobj_RecoBase
        lardataobj_RawData
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Persistency_Provenance canvas_Persistency_Provenance
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )

simple_plugin(LaserHitFinderTest "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        lardataobj_RecoBase
        lardataobj_RawData
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Persistency_Provenance canvas_Persistency_Provenance
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )

simple_plugin(LaserObjectsTest "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        lardataobj_RecoBase
        lardataobj_RawData
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Persistency_Provenance canvas_Persistency_Provenance
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )

simple_plugin(LaserSpotterTest "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        lardataobj_RecoBase
        lardataobj_RawData
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Persistency_Provenance canvas_Persistency_Provenance
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )

simple_plugin(LaserHitGenerator "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        lardataobj_RecoBase
        lardataobj_RawData
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Persistency_Provenance canvas_Persistency_Provenance
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )

simple_plugin(LaserRawDigitGenerator "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        lardataobj_RecoBase
        lardataobj_RawData
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Persistency_Provenance canvas_Persistency_Provenance
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )

simple_plugin(LaserRawDigitGenTest "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        lardataobj_RecoBase
        lardataobj_RawData
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Persistency_Provenance canvas_Persistency_Provenance
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )

simple_plugin(LaserSpotterGenTest "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        lardataobj_RecoBase
        lardataobj_RawData
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Persistency_Provenance canvas_Persistency_Provenance
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )

simple_plugin(LaserRecoTest "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        lardataobj_RecoBase
        lardataobj_RawData
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Persistency_Provenance canvas_Persistency_Provenance
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )

simple_plugin(LaserMergerTest "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        lardataobj_RecoBase
        lardataobj_RawData
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Persistency_Provenance canvas_Persistency_Provenance
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )

simple_plugin(LaserBeamTest "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_ArtDataHelper
        nusimdata_SimulationBase
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${CETLIB}
        ${ROOT_BASIC_LIB_LIST}
        BASENAME_ONLY
        )


#cet_test( LaserUtilsTest HANDBUILT
#        TEST_EXEC lar
#        TEST_ARGS -c LaserUtilsTest.fcl $ENV{MRB_TOP}/runs/TestEvent.root
#        )

#cet_test( LaserHitFinderTest HANDBUILT
#        TEST_EXEC lar
#        TEST_ARGS -c LaserHitFinderTest.fcl $ENV{MRB_TOP}/runs/TestEvent.root
#        )

#cet_test( LaserObjectsTest HANDBUILT
#        TEST_EXEC lar
#        TEST_ARGS -c LaserObjectsTest.fcl $ENV{MRB_TOP}/runs/TestLaser.root
#        )

cet_test( RawDigit_SingleGeneration HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserRawDigitGeneratorTest1.fcl
        DATAFILES ./spotter/RawDigits_Test_SingleGeneration.txt
        )

cet_test( RawDigit_MultiGeneration HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserRawDigitGeneratorTest2.fcl
        DATAFILES ./spotter/RawDigits_Test_MultiGeneration.txt
        )

cet_test( RawDigit_MultiplicityGeneration HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserRawDigitGeneratorTest3.fcl
        DATAFILES ./spotter/RawDigits_Test_MultiplicityGeneration.txt
        )

cet_test( RawDigit_MultiplicityError HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserRawDigitGeneratorTest4.fcl
        DATAFILES ./spotter/RawDigits_Test_MultiplicityError.txt
        )
set_tests_properties(RawDigit_MultiplicityError PROPERTIES PASS_REGULAR_EXPRESSION Multiplicity error @)

cet_test( LaserBeam_Test HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserBeamTest.fcl
        )

cet_test( LaserMerger_Merging_LCS1 HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserMerger_TestMerging_LCS1.fcl
        DATAFILES ./merger/WireIndexMap.root ./merger/HitDefs-10000.txt ./merger/Run-10000.txt ./merger/TimeMap-10000.root
        )

cet_test( LaserMerger_Merging_LCS2 HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserMerger_TestMerging_LCS2.fcl
        DATAFILES ./merger/WireIndexMap.root ./merger/HitDefs-10001.txt ./merger/Run-10001.txt ./merger/TimeMap-10001.root
        )

cet_test( LaserMerger_Mapping HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserMerger_TestMapping.fcl
        DATAFILES ./merger/WireIndexMap.root ./merger/HitDefs-10002.txt ./merger/Run-10002.txt ./merger/TimeMap-10002.root
        )

cet_test( LaserSpotter_HitLevels HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserSpotterGenHitsTest_Levels.fcl
        DATAFILES ./spotter/RawDigits_HitLevel.txt ./spotter/TimeMap-999999.root ./spotter/Run-999999.txt
        )

cet_test( LaserSpotter_ManualBox HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserSpotterGenHitsManual.fcl
        DATAFILES ./spotter/RawDigits_Threshold.txt ./spotter/TimeMap-999999.root ./spotter/Run-999999.txt
        )

cet_test( LaserSpotter_Threshold HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserSpotterGenHitsTest_Threshold.fcl
        DATAFILES ./spotter/RawDigits_Threshold.txt ./spotter/TimeMap-999999.root ./spotter/Run-999999.txt
        )

cet_test( LaserReco_SingleTrack HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserReco_TestSingleTrack.fcl
        DATAFILES ./reco/WireIndexMap.root ./reco/HitDefs-10000.txt ./reco/Run-10000.txt ./reco/TimeMap-10000.root
        )

cet_test( LaserReco_VerticalSpread HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserReco_TestVerticalSpread.fcl
        DATAFILES ./reco/WireIndexMap.root ./reco/HitDefs-10001.txt ./reco/Run-10001.txt ./reco/TimeMap-10001.root
        )

cet_test( LaserReco_TestHorizontalSpread HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserReco_TestHorizontalSpread.fcl
        DATAFILES ./reco/WireIndexMap.root ./reco/HitDefs-10002.txt ./reco/Run-10002.txt ./reco/TimeMap-10002.root
        )

cet_test( LaserReco_TestROI HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserReco_TestROI.fcl
        DATAFILES ./reco/WireIndexMap.root ./reco/HitDefs-10003.txt ./reco/Run-10003.txt ./reco/TimeMap-10003.root
        )


#cet_test( LaserBeamTest LIBRARIES LaserObjects )

# Add test items here
#cet_test( LaserSpotterTest1 HANDBUILT
#        TEST_EXEC lar
#        TEST_ARGS -c LaserSpotterTest.fcl
# $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3165-050.root
#        )
#cet_test( LaserSpotterTest2 HANDBUILT
#        TEST_EXEC lar
#        TEST_ARGS -c LaserSpotterTest.fcl
#        $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3165-051.root
#        )

#cet_test( LaserSpotterTest3 HANDBUILT
#        TEST_EXEC lar
#        TEST_ARGS -c LaserSpotterTest.fcl
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3165-251.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3165-257.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3165-051.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3165-516.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3165-517.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3165-518.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3165-519.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3165-521.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3007-104.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3007-111.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3007-132.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3007-133.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3007-151.root
#            $ENV{MRB_SOURCE}/laserana/test/spotter/LaserRun-3007-154.root
#        )


add_subdirectory(truncator)

install_headers()
install_fhicl()
install_source()
