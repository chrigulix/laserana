# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( $ENV{NUTOOLS_INC} )

art_make( BASENAME_ONLY
	  LIBRARY_NAME  LaserObjects
          LIB_LIBRARIES larcore_Geometry_Geometry_service
                        larcore_Geometry
			lardata_ArtDataHelper
			lardataobj_RecoBase
			lardataobj_RawData
                	${ROOT_BASIC_LIB_LIST}
                        ${ROOT_EG}
                        ${Boost_SYSTEM_LIBRARY}
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
  DICT_LIBRARIES
                        LaserObjects
)

install_headers()
install_fhicl()
install_source()