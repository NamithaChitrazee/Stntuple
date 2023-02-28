///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __Stntuple_mod_InitTrackSeedBlock__
#define __Stntuple_mod_InitTrackSeedBlock__

#include <string.h>
#include "canvas/Utilities/InputTag.h"

#include "Stntuple/obj/TStnInitDataBlock.hh"
#include "Stntuple/obj/TStnHelixBlock.hh"

#ifndef __CINT__

#include "Offline/TrackerGeom/inc/Tracker.hh"
#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/KalSeed.hh"
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

class StntupleInitTrackSeedBlock : public TStnInitDataBlock {
public:

  art::InputTag   fHsBlockName;      // helix seed  block name
  art::InputTag   fKsfCollTag;
  art::InputTag   fSdmcCollTag;
  art::InputTag   fSschCollTag;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:

  void SetKsfCollTag       (std::string&   Tag) { fKsfCollTag  = art::InputTag(Tag); }
  void SetSdmcCollTag      (art::InputTag& Tag) { fSdmcCollTag = Tag; }
  void SetSschCollTag      (art::InputTag& Tag) { fSschCollTag = Tag; }
  void SetHsBlockName      (std::string&  Name) { fHsBlockName = Name.data()       ; }

  virtual int InitDataBlock(TStnDataBlock* Block, AbsEvent* AnEvent, int Mode);
  virtual int ResolveLinks (TStnDataBlock* Block, AbsEvent* AnEvent, int Mode);

};

#endif
