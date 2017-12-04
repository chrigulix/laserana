include_directories ( $ENV{GEANT4_FQ_DIR}/include )

art_make( MODULE_LIBRARIES
			LaserObjects
			larsim_LArG4

		larsim_Simulation
		lardataobj_Simulation
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
			${G4_LIB_LIST}
			CLHEP
)

install_headers()
install_fhicl()
install_source()
