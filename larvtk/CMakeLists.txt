# basic source code CMakeLists.txt


art_make(
        MODULE_LIBRARIES
        larvtk_vtkData
        larcore_Geometry
        artvtk_artvtk
        MF_messageLogger
        art_Framework_Services_Registry
)

install_headers()
install_source()
install_fhicl()
