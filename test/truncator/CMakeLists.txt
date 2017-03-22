# cmake driver file for testing

# Enable asserts
include(CetTest)
cet_enable_asserts()

cet_test( TruncatorTest_NoTruncation HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c TruncatorTest_NoTruncation.fcl
        DATAFILES ./NoTrunc.txt
        )

cet_test( TruncatorTest_DropInFront HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c TruncatorTest_DropInFront.fcl
        DATAFILES ./OnlyFront.txt
        )

cet_test( TruncatorTest_DropFrontAndBack HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c TruncatorTest_DropFrontAndBack.fcl
        DATAFILES ./FullTrunc.txt
        )

cet_test( TruncatorTest_DropMultiWire HANDBUILT
        TEST_EXEC lar
        TEST_ARGS -c TruncatorTest_DropMultiWire.fcl
        DATAFILES ./MultiWires.txt
        )


simple_plugin(TruncTest "module"
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

install_headers()
install_fhicl()
install_source()
