///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __Stntuple_mod_InitHelixBlock__
#define __Stntuple_mod_InitHelixBlock__

#include <string.h>
#include "canvas/Utilities/InputTag.h"

#include "Stntuple/obj/TStnInitDataBlock.hh"
#include "Stntuple/obj/TStnHelixBlock.hh"

#ifndef __CINT__

#include "Offline/TrackerGeom/inc/Tracker.hh"
#include "Offline/RecoDataProducts/inc/AlgorithmID.hh"
#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/HelixSeed.hh"
#include "Offline/MCDataProducts/inc/StrawDigiMC.hh"
#include "Offline/MCDataProducts/inc/StepPointMC.hh"

#else
namespace mu2e {
  class Tracker;
  class ComboHitCollection;
  class HelixSeedCollection;
  class StrawDigiMCCollection;
  class StepPointMCCollection;
};
#endif

class StntupleInitHelixBlock : public TStnInitDataBlock {
public:

  art::InputTag   fAlgorithmIDCollTag;
  art::InputTag   fSdmcCollTag;
  art::InputTag   fHSeedCollTag;        // helix seed  coll tag
  art::InputTag   fKsfCollTag;          // helix KSF   coll tag, to find assns
  TString         fKsfBlockName;        // KalSeedFit  block name
  TString         fTclBlockName;        // TimeCluster block name

  const mu2e::HelixSeedCollection*         fListOfHSeeds;

  mu2e::AlgorithmIDCollection*             list_of_algs               ;
  const mu2e::StrawDigiMCCollection*       list_of_mc_straw_hits      ;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:

  void   SetHSeedCollTag   (std::string& Tag ) { fHSeedCollTag = art::InputTag(Tag); }
  void   SetSdmcCollTag    (art::InputTag& Tag) { fSdmcCollTag = Tag; }
  void   SetKsfCollTag     (art::InputTag& Tag) { fKsfCollTag = Tag; }
  void   SetKsfBlockName   (std::string& Name) { fKsfBlockName = Name.data()       ; }
  void   SetTclBlockName   (std::string& Name) { fTclBlockName = Name.data()       ; }

  virtual int InitDataBlock(TStnDataBlock* Block, AbsEvent* Evt, int Mode);
  virtual int ResolveLinks (TStnDataBlock* Block, AbsEvent* Evt, int Mode);

};

#endif
