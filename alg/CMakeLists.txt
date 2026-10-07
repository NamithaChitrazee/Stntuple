#
SET (CMAKE_Fortran_COMPILER  gfortran)
ENABLE_LANGUAGE(Fortran)

set (subpkg    "alg")
set (dict      "Stntuple_${subpkg}_G")
set (lib       "Stntuple_${subpkg}") 

include_directories(
  $ENV{ROOTSYS}/include/root
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/${subpkg}
)

# link_directories($ENV{ROOTSYS}/lib)

ROOT_GENERATE_DICTIONARY(${dict}
  smooth.hh TEmuLogLH.hh TStnTrackID.hh TStntuple.hh TStnPrintUtils.hh
  LINKDEF ${subpkg}/Linkdef.h)

set_property(SOURCE ${dict}.cxx PROPERTY generated)

add_library(${lib} SHARED
  InitPbarCommon.cc RMCSpectra.cc smooth.cc TEmuLogLH.cc TStnTrackID.cc TStntuple.cc TStntuple_Pbar_Striganov.cc
  pbar_common_address.f pbar.f
  TStnPrintUtils.cc
  ${dict}.cxx
)
add_library(Stntuple::alg ALIAS ${lib}  )

target_include_directories(${lib} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS ${lib}
  EXPORT StntupleTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include
)

install(EXPORT StntupleTargets
  FILE StntupleTargets.cmake
  NAMESPACE Stntuple::
  DESTINATION lib/cmake/Stntuple
)

target_link_libraries(${lib}
  Offline::DataProducts
  fhiclcpp::fhiclcpp
  cetlib::cetlib
  cetlib_except::cetlib_except
  ROOT::Core ROOT::Matrix ROOT::Tree ROOT::Graf ROOT::Gpad
)

target_include_directories(${lib}
#  PUBLIC  ${art_INCLUDE_DIR}
  PRIVATE ${art_INCLUDE_DIR}
  PRIVATE ${canvas_INCLUDE_DIR}
)

cet_add_compiler_flags(-g -O0)

install(TARGETS ${lib})

# install_headers()
# install_source()

install(DIRECTORY ${subpkg} DESTINATION include/Stntuple)
