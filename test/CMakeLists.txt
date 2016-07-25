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

cet_test( LaserUtilsTest HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c LaserUtilsTest.fcl $ENV{MRB_TOP}/runs/TestEvent.root
        )



# Add test items here
cet_test( LaserSpotter_test LIBRARIES LaserObjects )
#cet_test( LaserUtils_test LIBRARIES LaserObjects )

install_headers()
install_fhicl()
install_source()
