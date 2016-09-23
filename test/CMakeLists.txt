# cmake driver file for testing

# Enable asserts
include(CetTest)
cet_enable_asserts()

# For LaserUtils test
simple_plugin(LaserUtilsTest "module"
        LaserObjects
        larcore_Geometry_Geometry_service
        larcore_Geometry
        lardata_RecoBaseArt
        lardata_RecoBase
        lardata_RawData
        ${SIMULATIONBASE}
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        ${ART_PERSISTENCY_COMMON}
        ${ART_PERSISTENCY_PROVENANCE}
        ${ART_UTILITIES}
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
        lardata_RecoBaseArt
        lardata_RecoBase
        lardata_RawData
        ${SIMULATIONBASE}
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        ${ART_PERSISTENCY_COMMON}
        ${ART_PERSISTENCY_PROVENANCE}
        ${ART_UTILITIES}
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
        lardata_RecoBaseArt
        lardata_RecoBase
        lardata_RawData
        ${SIMULATIONBASE}
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        ${ART_PERSISTENCY_COMMON}
        ${ART_PERSISTENCY_PROVENANCE}
        ${ART_UTILITIES}
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
        lardata_RecoBaseArt
        lardata_RecoBase
        lardata_RawData
        ${SIMULATIONBASE}
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        ${ART_PERSISTENCY_COMMON}
        ${ART_PERSISTENCY_PROVENANCE}
        ${ART_UTILITIES}
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
        lardata_RecoBaseArt
        lardata_RecoBase
        lardata_RawData
        ${SIMULATIONBASE}
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        ${ART_PERSISTENCY_COMMON}
        ${ART_PERSISTENCY_PROVENANCE}
        ${ART_UTILITIES}
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
        lardata_RecoBaseArt
        lardata_RecoBase
        lardata_RawData
        ${SIMULATIONBASE}
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        ${ART_PERSISTENCY_COMMON}
        ${ART_PERSISTENCY_PROVENANCE}
        ${ART_UTILITIES}
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
        lardata_RecoBaseArt
        lardata_RecoBase
        lardata_RawData
        ${SIMULATIONBASE}
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        ${ART_PERSISTENCY_COMMON}
        ${ART_PERSISTENCY_PROVENANCE}
        ${ART_UTILITIES}
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
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
        DATAFILES RawDigits_Test_SingleGeneration.txt
        )

cet_test( RawDigit_MultiGeneration HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserRawDigitGeneratorTest2.fcl
        DATAFILES RawDigits_Test_MultiGeneration.txt
        )

cet_test( RawDigit_MultiplicityGeneration HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserRawDigitGeneratorTest3.fcl
        DATAFILES RawDigits_Test_MultiplicityGeneration.txt
        )

cet_test( RawDigit_MultiplicityError HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserRawDigitGeneratorTest4.fcl
        DATAFILES RawDigits_Test_MultiplicityError.txt
        )
set_tests_properties(RawDigit_MultiplicityError PROPERTIES PASS_REGULAR_EXPRESSION Multiplicity error @)

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
#cet_test( LaserUtils_test LIBRARIES LaserObjects )

install_headers()
install_fhicl()
install_source()
