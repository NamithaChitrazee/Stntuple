///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __STNTUPLE_InitSimpBlock__
#define __STNTUPLE_InitSimpBlock__

#include <string.h>

#include "canvas/Utilities/InputTag.h"

#include "Stntuple/obj/TStnInitDataBlock.hh"
#include "Stntuple/obj/TSimpBlock.hh"

#include "Mu2eUtilities/inc/SimParticleTimeOffset.hh"

class StntupleInitSimpBlock : public TStnInitDataBlock {
public:
  art::InputTag   fSimpCollTag;
  art::InputTag   fStrawHitCollTag;
  art::InputTag   fStrawDigiMCCollTag;
  art::InputTag   fVDHitsCollTag;
  art::InputTag   fPrimaryParticleTag;
  float           fMinSimpMomentum;
  float           fMaxZ;
  int             fGenProcessID;
  int             fPdgID;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:

  void   SetSimpCollTag       (std::string& Tag ) { fSimpCollTag        = art::InputTag(Tag); }
  void   SetStrawHitCollTag   (std::string& Tag ) { fStrawHitCollTag    = art::InputTag(Tag); }
  void   SetStrawDigiMCCollTag(std::string& Tag ) { fStrawDigiMCCollTag = art::InputTag(Tag); }
  void   SetVDHitsCollTag     (std::string& Tag ) { fVDHitsCollTag      = art::InputTag(Tag); }
  void   SetPrimaryParticleTag(std::string& Tag ) { fPrimaryParticleTag = art::InputTag(Tag); }
  void   SetMinSimpMomentum   (double       MinP) { fMinSimpMomentum    = MinP              ; }
  void   SetMaxZ              (double       MaxZ) { fMaxZ               = MaxZ              ; }
  void   SetGenProcessID      (int          ID  ) { fGenProcessID       = ID                ; }
  void   SetPdgID             (int          ID  ) { fPdgID              = ID                ; }

  void   SetTimeOffsets       (mu2e::SimParticleTimeOffset* TimeOffsets) { fTimeOffsets = TimeOffsets; }
  void   SetMbTime            (float MbTime) { fMbTime = MbTime; }

  virtual int InitDataBlock(TStnDataBlock* Block, AbsEvent* Evt, int Mode);
  //  virtual int ResolveLinks (TStnDataBlock* Block, AbsEvent* Evt, int Mode);

};

#endif
