///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __STNTUPLE_InitGenpBlock__
#define __STNTUPLE_InitGenpBlock__

#include <string.h>

#include "canvas/Utilities/InputTag.h"

#include "Stntuple/obj/TStnInitDataBlock.hh"
#include "Stntuple/obj/TSimpBlock.hh"

class StntupleInitGenpBlock : public TStnInitDataBlock {
public:
  art::InputTag   fGenpCollTag;
  int             fGenProcessID;
  int             fPdgID;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:

  void   SetGenpCollTag    (art::InputTag& Tag) { fGenpCollTag  = Tag; }
  void   SetGenProcessID   (int            ID ) { fGenProcessID = ID ; }
  void   SetPdgID          (int            ID ) { fPdgID        = ID ; }

  virtual int InitDataBlock(TStnDataBlock* Block, AbsEvent* Evt, int Mode);
  // virtual int ResolveLinks (TStnDataBlock* Block, AbsEvent* Evt, int Mode);

};

#endif
