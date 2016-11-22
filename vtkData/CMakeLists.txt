# basic source code CMakeLists.txt

art_make(
        LIB_LIBRARIES
        lardataobj_RecoBase
        larcore_Geometry
        larcore_Geometry_Geometry_service
        ${VTK_LIBRARIES}
        ${ART_FRAMEWORK_CORE}
        ${ART_FRAMEWORK_PRINCIPAL}
        ${ART_FRAMEWORK_SERVICES_REGISTRY}
        ${ART_FRAMEWORK_SERVICES_BASIC}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
        ${ART_FRAMEWORK_SERVICES_OPTIONAL_RANDOMNUMBERGENERATOR_SERVICE}
        art_Persistency_Common canvas_Persistency_Common
        art_Utilities canvas_Utilities
        ${MF_MESSAGELOGGER}
        ${MF_UTILITIES}
        ${FHICLCPP}
        ${CETLIB}
        ${CLHEP}
        ${ROOT_BASIC_LIB_LIST}
        ${Boost_SYSTEM_LIBRARY})

install_headers()
install_source()
install_fhicl()
