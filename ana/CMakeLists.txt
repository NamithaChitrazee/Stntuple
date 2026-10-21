#
set (subpkg "ana")
set (dict   "Stntuple_${subpkg}_dict")
set (lib    "Stntuple_${subpkg}")

include_directories(
  $ENV{ROOTSYS}/include/root
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/${subpkg}
)

# link_directories($ENV{ROOTSYS}/lib)

set (include_files
    TClusterAnaModule.hh
    TCrvAnaModule.hh TCrvTimeAnaModule.hh
    TDetTimeAnaModule.hh
    TEventDisplayModule.hh 
    TGenAnaModule.hh
    THelixAnaModule.hh
    TSimpAnaModule.hh
    TSpmcAnaModule.hh
    TTrackAnaModule.hh
    TTrackSeedAnaModule.hh
    TTriggerAnaModule.hh
    TValidationModule.hh)

if ($ENV{STNTUPLE_MC_GEN})
  append (include_files TPhotosAnaModule.hh)
endif()

  
ROOT_GENERATE_DICTIONARY(${dict} ${include_files} LINKDEF ${subpkg}/Linkdef.h)

set_property(SOURCE ${dict}.cxx PROPERTY generated)

set (source_files
        InitVirtualDetectors.cc
        TClusterAnaModule.cc
        TCrvAnaModule.cc TCrvTimeAnaModule.cc
        TDetTimeAnaModule.cc
        TEventDisplayModule.cc
        TGenAnaModule.cc 
        THelixAnaModule.cc
        TSimpAnaModule.cc
        TSpmcAnaModule.cc
        TTrackAnaModule.cc
        TTrackSeedAnaModule.cc 
        TTriggerAnaModule.cc
        TValidationModule.cc)

if ($ENV{STNTUPLE_MC_GEN})
  append (source_files TPhotosAnaModule.cc)
endif()

add_library(${lib} SHARED ${source_files} ${dict}.cxx)

target_link_libraries(${lib}
  Stntuple::alg Stntuple::obj Stntuple::loop Stntuple::gui Stntuple::geom Stntuple::base
  ROOT::Core ROOT::Matrix ROOT::Tree ROOT::Graf ROOT::Gpad)

cet_add_compiler_flags(-g -O0)

install(TARGETS ${lib})
# install_headers()
# install_source()
install(DIRECTORY ${subpkg} DESTINATION include/Stntuple)
