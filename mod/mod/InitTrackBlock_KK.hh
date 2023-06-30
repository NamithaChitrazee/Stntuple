///////////////////////////////////////////////////////////////////////////////
// just redefine ResolveLinks function
///////////////////////////////////////////////////////////////////////////////
#ifndef __Stntuple_mod_InitTrackBlock_KK__
#define __Stntuple_mod_InitTrackBlock_KK__

#include "Stntuple/mod/InitTrackBlock.hh"

class StntupleInitTrackBlock_KK : public StntupleInitTrackBlock {
public:
  
  virtual int ResolveLinks (TStnDataBlock* Block, AbsEvent* Evt, int Mode);

};

#endif
