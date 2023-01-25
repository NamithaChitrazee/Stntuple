///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __InitTimeClusterBlock__
#define __InitTimeClusterBlock__

#include <string.h>

#include "canvas/Utilities/InputTag.h"

#include "Stntuple/obj/TStnInitDataBlock.hh"
#include "Stntuple/obj/TStnTimeClusterBlock.hh"

class StntupleInitTimeClusterBlock : public TStnInitDataBlock {
public:
  art::InputTag   fTimeClusterCollTag;
  art::InputTag   fShCollTag;
  art::InputTag   fChCollTag;
  art::InputTag   fStrawDigiMCCollTag;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:

  void   SetTimeClusterCollTag (std::string& Tag) { fTimeClusterCollTag = art::InputTag(Tag); }
  void   SetChCollTag    (art::InputTag& Tag) { fChCollTag    = Tag; }
  void   SetShCollTag    (art::InputTag& Tag) { fShCollTag    = Tag; }
  void   SetStrawDigiMCCollTag (std::string& Tag) { fStrawDigiMCCollTag = art::InputTag(Tag); }
  //  void   SetHelixCollTag       (std::string& Tag) { fHelixCollTag       = art::InputTag(Tag); }
  
  virtual int InitDataBlock    (TStnDataBlock* Block, AbsEvent* Evt, int Mode);
  virtual int ResolveLinks     (TStnDataBlock* Block, AbsEvent* Evt, int Mode);

};

#endif
