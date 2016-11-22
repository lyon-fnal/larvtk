# ======================================================================
#  larvtk main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larvtk/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(larvtk)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(CMAKE_MODULE_PATH $ENV{ART_DIR}/Modules
		      $ENV{CETBUILDTOOLS_DIR}/Modules
		      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

set (ART_MAKE_PREPEND_PRODUCT_NAME TRUE)

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  EXTRA_CXX_FLAGS -std=c++14 )

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( larsoft v1_00_00 )
find_ups_product( lardataobj v1_00_00 )
find_ups_product( larcore v1_00_00 )
find_ups_product( art v1_08_00 )
find_ups_product( cetbuildtools v4_14_01 )

# Load the ParaView stuff (ParaView_DIR is a magic cmake variable that tells find_package where to find build products for ParaView Catalyst)
get_filename_component(ParaView_DIR "$ENV{PARAVIEW_FQ_DIR}/lib/cmake/paraview-5.1" REALPATH)
set(USE_CATALYST ON CACHE BOOL "Link the simulation with Catalyst")
find_package(ParaView 5.1 REQUIRED COMPONENTS vtkPVPythonCatalyst vtkFiltersSources)
include_directories(${PARAVIEW_INCLUDE_DIRS})
add_definitions("-DUSE_CATALYST")

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# ADD SOURCE CODE SUBDIRECTORIES HERE
add_subdirectory(vtkData)
add_subdirectory(larvtk)

# tests
add_subdirectory(test)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
