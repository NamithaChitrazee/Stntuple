#ifndef STNTUPLE_TStrawHitBlock
#define STNTUPLE_TStrawHitBlock

#include "TClonesArray.h"

#include "Stntuple/obj/TStnDataBlock.hh"
#include "Stntuple/obj/TStrawHit.hh"

#include "Stntuple/mod/InitStntupleDataBlocks.hh"

class TStrawHitBlock: public TStnDataBlock {
  friend class stntuple::InitStrawHitBlock;

public:
  Int_t          fNHits;	        // number of hits in the straw tracker
  TClonesArray*  fListOfHits;		// list of hits
//-----------------------------------------------------------------------------
//  functions
//-----------------------------------------------------------------------------
public:
					// constructors and destructor
  TStrawHitBlock();
  virtual ~TStrawHitBlock();
					// accessors

  Int_t          NHits    () { return fNHits; }
  TStrawHit* Hit (int i) { return (TStrawHit*) fListOfHits->UncheckedAt(i); }
  
  TClonesArray* GetListOfHits () { return fListOfHits; }
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
                                        //Create hit, increse number of hits

  TStrawHit* NewHit() { return new ((*fListOfHits)[fNHits++]) TStrawHit(); } 
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  void Clear(Option_t* opt="");
  void Print(Option_t* opt="") const;

  ClassDef(TStrawHitBlock,1)	// straw hit data block
};


#endif


