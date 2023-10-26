///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __InitStepPointMCBlock__
#define __InitStepPointMCBlock__

#include <string.h>

#include "canvas/Utilities/InputTag.h"

#include "Stntuple/obj/TStnInitDataBlock.hh"

class StntupleInitStepPointMCBlock : public TStnInitDataBlock {
public:
  art::InputTag                fSpmcCollTag;
  art::InputTag                fStatusG4Tag;
  float                        fMbTime;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:

  void   SetSpmcCollTag(art::InputTag& Tag) { fSpmcCollTag = Tag; }
  void   SetStatusG4Tag(std::string&   Tag) { fStatusG4Tag = art::InputTag(Tag); }

  void   SetMbTime            (float MbTime) { fMbTime = MbTime; }
  
  virtual int InitDataBlock(TStnDataBlock* Block, AbsEvent* Evt, int Mode);
  virtual int ResolveLinks (TStnDataBlock* Block, AbsEvent* Evt, int Mode);

};

#endif
