///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __InitStepPointMCBlock__
#define __InitStepPointMCBlock__

#include <string.h>

#include "canvas/Utilities/InputTag.h"

#include "Offline/Mu2eUtilities/inc/SimParticleTimeOffset.hh"

#include "Stntuple/obj/TStnInitDataBlock.hh"

class StntupleInitStepPointMCBlock : public TStnInitDataBlock {
public:
  art::InputTag                fSpmcCollTag;
  art::InputTag                fStatusG4Tag;
  mu2e::SimParticleTimeOffset* fTimeOffsets;
  float                        fMbTime;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:

  void   SetSpmcCollTag(std::string& Tag) { fSpmcCollTag = art::InputTag(Tag); }
  void   SetStatusG4Tag(std::string& Tag) { fStatusG4Tag = art::InputTag(Tag); }

  void   SetTimeOffsets       (mu2e::SimParticleTimeOffset* TimeOffsets) { fTimeOffsets = TimeOffsets; }

  void   SetMbTime            (float MbTime) { fMbTime = MbTime; }
  
  virtual int InitDataBlock(TStnDataBlock* Block, AbsEvent* Evt, int Mode);
  virtual int ResolveLinks (TStnDataBlock* Block, AbsEvent* Evt, int Mode);

};

#endif
