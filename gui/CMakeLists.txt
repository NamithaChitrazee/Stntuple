#
set (subpkg "gui"                    )
set (dict   "Stntuple_${subpkg}_dict")
set (lib    "Stntuple_${subpkg}"     )

include_directories(
  $ENV{ROOTSYS}/include/root
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/${subpkg}
)

# link_directories($ENV{ROOTSYS}/lib)

set (include_files
  DisplayMu2eGeometry.hh TCalView.hh TCalVisNode.hh TCrvView.hh TCrvVisNode.hh
  TEvdCalSection.hh TEvdCluster.hh TEvdComboHit.hh TEvdCrvBar.hh TEvdCrvSection.hh
  TEvdCrystal.hh TEvdHelixSeed.hh TEvdHelixVisNode.hh TEvdMainFrame.hh
  TEvdPanel.hh TEvdPlane.hh TEvdSimParticle.hh TEvdStation.hh TEvdStrawHit.hh TEvdStraw.hh
  TEvdStrawTracker.hh TEvdTimeCluster.hh TEvdTimeClusterVisNode.hh TEvdTrack.hh
  TEvdTrkStrawHit.hh TEventDisplayUtils.hh THeaderVisNode.hh TMcTruthVisNode.hh
  TMu2eBField.hh TStnFrame.hh TStnGeoManager.hh TStnVisManager.hh
  TStnVisNode.hh TTrkVisNode.hh
)

ROOT_GENERATE_DICTIONARY(${dict} ${include_files} LINKDEF ${subpkg}/Linkdef.h)

set_property(SOURCE ${dict}.cxx PROPERTY generated)

set (source_files
  DisplayMu2eGeometry.cc TCalView.cc TCalVisNode.cc TCrvView.cc TCrvVisNode.cc
  TEvdCalSection.cc TEvdCluster.cc TEvdComboHit.cc TEvdCrvBar.cc TEvdCrvSection.cc
  TEvdCrystal.cc TEvdHelixSeed.cc TEvdHelixVisNode.cc TEvdMainFrame.cc TEvdPanel.cc
  TEvdPlane.cc TEvdSimParticle.cc TEvdStation.cc TEvdStraw.cc TEvdStrawHit.cc
  TEvdStrawTracker.cc TEvdTimeCluster.cc TEvdTimeClusterVisNode.cc TEvdTrack.cc
  TEvdTrkStrawHit.cc TEventDisplayUtils.cc THeaderVisNode.cc TMcTruthVisNode.cc
  TMu2eBField.cc TStnFrame.cc TStnGeoManager.cc TStnVisManager.cc TStnVisNode.cc
  TTrkVisNode.cc
)

add_library(${lib} SHARED ${source_files} ${dict}.cxx)

target_link_libraries(${lib}
  Stntuple_print
  Stntuple_geom
  Stntuple_obj
  Stntuple_base
  
  Offline::RecoDataProducts
  Offline::MCDataProducts
  Offline::DataProducts
  Offline::CalorimeterGeom
  
  art::Framework_Core
  art::Framework_Services_Registry
  art::Framework_Principal
  art::Persistency_Common
  art::Persistency_Provenance
  art::Utilities

  ROOT::Physics ROOT::GenVector ROOT::Eve ROOT::Graf ROOT::Graf3d ROOT::Gui ROOT::Gpad
  ROOT::Geom ROOT::Matrix ROOT::Core
)

cet_add_compiler_flags(-g -O0)

install(TARGETS ${lib})
install_headers()
# install_source()
install(DIRECTORY ${subpkg} DESTINATION include/Stntuple)
