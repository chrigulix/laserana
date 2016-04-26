# for classes_def.xml!!
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( $ENV{NUTOOLS_INC} )

art_make( BASENAME_ONLY
	  LIBRARY_NAME  LaserObjects
          LIB_LIBRARIES 
			Geometry_service
                        Geometry
			RecoBaseArt
			RecoBase
			RawData
                	${ROOT_BASIC_LIB_LIST}
                        ${ROOT_EG}
                        ${Boost_SYSTEM_LIBRARY}
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
)

install_headers()
install_fhicl()
install_source()