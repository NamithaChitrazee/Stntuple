///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __InitStrawHitBlock__
#define __InitStrawHitBlock__

#include <string.h>

#include "canvas/Utilities/InputTag.h"

#include "Offline/TrackerConditions/inc/TrackerPanelMap.hh"
#include "Offline/ProditionsService/inc/ProditionsHandle.hh"

#include "Stntuple/obj/TStnInitDataBlock.hh"
#include "Stntuple/obj/TStrawHitBlock.hh"

namespace stntuple {
class InitStrawHitBlock : public TStnInitDataBlock {
public:
  art::InputTag   fShCollTag;
  art::InputTag   fStrawDigiCollTag;
  art::InputTag   fStrawDigiMCCollTag;
  art::InputTag   fTimeClusterCollTag;
  int             fWriteSdwf;
  bool            fStoreCloseHitsOnly;      // if 1, store only "close" hits
  float           fMinEDep;
  float           fMinDt;
  
  int             fLastRun;

  mu2e::ProditionsHandle<mu2e::TrackerPanelMap>  fTpmh;
  const mu2e::TrackerPanelMap*                   fTpm;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:

  InitStrawHitBlock();

  void   SetShCollTag          (art::InputTag& Tag) { fShCollTag          = Tag;  }
  void   SetStrawDigiCollTag   (art::InputTag& Tag) { fStrawDigiCollTag   = Tag;  }
  void   SetStrawDigiMCCollTag (art::InputTag& Tag) { fStrawDigiMCCollTag = Tag;  }
  void   SetTimeClusterCollTag (art::InputTag& Tag) { fTimeClusterCollTag = Tag;  }
  void   SetWriteSdwf          (int           Flag) { fWriteSdwf          = Flag; }
  void   SetStoreCloseHitsOnly (bool          Flag) { fStoreCloseHitsOnly = Flag; }
  void   SetMinEDep            (float         EDep) { fMinEDep            = EDep; }
  void   SetMinDt              (float         Dt  ) { fMinDt              = Dt;   }
  
  virtual int InitDataBlock  (TStnDataBlock* Block, AbsEvent* Evt, int Mode);
  //  virtual int ResolveLinks   (TStnDataBlock* Block, AbsEvent* Evt, int Mode);

};
}
#endif
