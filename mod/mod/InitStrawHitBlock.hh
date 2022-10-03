///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#ifndef __InitStrawHitBlock__
#define __InitStrawHitBlock__

#include <string.h>

#include "canvas/Utilities/InputTag.h"

#include "Stntuple/obj/TStnInitDataBlock.hh"
#include "Stntuple/obj/TStrawHitBlock.hh"

namespace stntuple {
class InitStrawHitBlock : public TStnInitDataBlock {
public:
  art::InputTag   fStrawHitCollTag;
  art::InputTag   fStrawDigiCollTag;
  art::InputTag   fStrawDigiMCCollTag;
  art::InputTag   fSdwfCollTag;
  int             fWriteSdwf;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
public:

  void   SetStrawHitCollTag    (std::string& Tag) { fStrawHitCollTag    = art::InputTag(Tag); }
  void   SetStrawDigiCollTag   (std::string& Tag) { fStrawDigiCollTag   = art::InputTag(Tag); }
  void   SetStrawDigiMCCollTag (std::string& Tag) { fStrawDigiMCCollTag = art::InputTag(Tag); }
  void   SetSdwfCollTag        (std::string& Tag) { fSdwfCollTag        = art::InputTag(Tag); }
  void   SetWriteSdwf          (int         Flag) { fWriteSdwf          = Flag              ; }
  
  virtual int InitDataBlock  (TStnDataBlock* Block, AbsEvent* Evt, int Mode);
  //  virtual int ResolveLinks   (TStnDataBlock* Block, AbsEvent* Evt, int Mode);

};
}
#endif
