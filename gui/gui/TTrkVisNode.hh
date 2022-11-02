#ifndef Stntuple_gui_TTrkVisNode_hh
#define Stntuple_gui_TTrkVisNode_hh

#include "Gtypes.h"
#include "TClonesArray.h"
#include "TArc.h"

#include "art/Framework/Principal/Event.h"
					// has to be here because of typedef's
#include "Offline/RecoDataProducts/inc/KalSeed.hh"

#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/StrawDigi.hh"
#include "Offline/RecoDataProducts/inc/StrawHit.hh"
#include "Offline/MCDataProducts/inc/StrawDigiMC.hh"
#include "Offline/MCDataProducts/inc/SimParticle.hh"
#include "Offline/MCDataProducts/inc/StepPointMC.hh"

#include "Stntuple/gui/TStnVisNode.hh"

class TStnTrackBlock;
class TSimpBlock;

namespace mu2e {
  class TimeCluster;
  class Tracker;
}

namespace stntuple {
  class TEvdStrawTracker;
  class TEvdStrawHit;
  class TEvdComboHit;
  class TEvdTrack;
  class TEvdSimParticle;
}

class TTrkVisNode: public TStnVisNode {
public:
  enum {
    kPickHits     = 0,
    kPickTracks   = 1,
    kPickClusters = 2
  };
  
protected:

  
  const mu2e::ComboHitCollection*              fChColl;     // several SH per CH
  std::string                                  fChCollTag;
  
  const mu2e::ComboHitCollection*              fSchColl;     // one     SH per CH
  const mu2e::StrawHitCollection*              fShColl;            // straw hit coll
  std::string                                  fShCollTag;

  const mu2e::StrawDigiCollection**            fSdColl;		   // straw digi coll

  const mu2e::StrawDigiMCCollection*           fSdmcColl;          // straw digi MC coll
  std::string                                  fSdmcCollTag;       // straw digi collection

  const mu2e::StrawDigiADCWaveformCollection** fSwColl;		   // straw digi waveform coll

  const mu2e::KalSeedCollection*               fKsColl;
  std::string                                  fKsCollTag;         // straw digi collection

  const mu2e::SimParticleCollection**          fSimpColl;
  const mu2e::StepPointMCCollection**          fSpmcColl;

  TStnTrackBlock*           fTrackBlock;
  Color_t                   fTrackColor;

  stntuple::TEvdStrawTracker* fTracker;

  TArc*                     fArc;

  Int_t                     fTimeWindow;
  Int_t                     fPickMode;
  Int_t                     fUseStereoHits;
  double                    fEventTime;
		            
  TObjArray*                fListOfComboHits;
  TObjArray*                fListOfStrawHits;
  TObjArray*                fListOfTracks;
  TObjArray*                fListOfSimParticles;

  TSimpBlock*               fSimpBlock;
public:
//-----------------------------------------------------------------------------
// constructors and destructor
//-----------------------------------------------------------------------------
  TTrkVisNode();
  TTrkVisNode(const char* Name, const mu2e::Tracker* Tracker, TStnTrackBlock* fTrackBlock);
  virtual ~TTrkVisNode();

//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  TObjArray* GetListOfTracks() { return fListOfTracks; }
  Color_t    GetTrackColor  () { return fTrackColor;   }

  int        GetNTracks()      { return fListOfTracks->GetEntriesFast(); }
  int        GetNStrawHits  () { return fListOfStrawHits->GetEntriesFast(); }
  int        GetNComboHits  () { return fListOfComboHits->GetEntriesFast(); }

  stntuple::TEvdComboHit* GetEvdComboHit(int I) { 
    return (stntuple::TEvdComboHit*) fListOfComboHits->At(I); 
  }

  stntuple::TEvdStrawHit* GetEvdStrawHit(int I) { 
    return (stntuple::TEvdStrawHit*) fListOfStrawHits->At(I); 
  }

  stntuple::TEvdTrack*    GetEvdTrack   (int I) { 
    return (stntuple::TEvdTrack*)    fListOfTracks->At(I); 
  }

  stntuple::TEvdSimParticle* GetEvdSimParticle(int I) { 
    return (stntuple::TEvdSimParticle*)  fListOfSimParticles->At(I); 
  }

  const mu2e::ComboHitCollection* GetCComboHitColl() { return  fChColl; }
  //  const mu2e::ComboHitCollection* GetSComboHitColl() { return  fSchColl; }

  // const mu2e::StrawHitFlagCollection* GetStrawHitFlagColl() { 
  //   return *fStrawHitFlagColl;
  // }

  virtual void NodePrint(const void* Object, const char* ClassName) ;
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  //  void  SetInitSimpBlock(StntupleInitSimpBlock* InitBlock) { fInitSimpBlock = InitBlock; }

  void SetKsCollTag     (std::string& CollTag) { fKsCollTag   = CollTag; }
  void SetChCollTag     (std::string& CollTag) { fChCollTag   = CollTag; }
  void SetShCollTag     (std::string& CollTag) { fShCollTag   = CollTag; }
  void SetSdmcCollTag   (std::string& CollTag) { fSdmcCollTag = CollTag; }

  void  SetSimpBlock   (TSimpBlock* Block) { fSimpBlock    = Block; }
  void  SetListOfTracks(TObjArray*  List ) { fListOfTracks = List ; }
  void  SetTrackColor  (Color_t     Color) { fTrackColor   = Color; }

  void SetSwColl(const mu2e::StrawDigiADCWaveformCollection** Coll) { 
    fSwColl = Coll;
  }

  // void SetStrawHitFlagColl(const mu2e::StrawHitFlagCollection** Coll) { 
  //   fStrawHitFlagColl = Coll;
  // }

  void SetSimpColl(const mu2e::SimParticleCollection** Coll) { 
    fSimpColl = Coll;
  }

  void SetSpmcColl(const mu2e::StepPointMCCollection** Coll) { 
    fSpmcColl = Coll;
  }

  void  SetPickMode   (Int_t Mode) { fPickMode    = Mode; }

					// Index - in fShColl
  int   TCHit(const mu2e::TimeCluster* TimeCluster, int Index);
//-----------------------------------------------------------------------------
// overloaded methods of TVisNode
//-----------------------------------------------------------------------------
  virtual int  InitEvent();

  virtual void PaintXY (Option_t* option = "");
  virtual void PaintRZ (Option_t* option = "");
  virtual void PaintTZ (Option_t* option = "");
  virtual void PaintVST(Option_t* option = "");
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  virtual int  DistancetoPrimitiveXY(Int_t px, Int_t py);
  virtual int  DistancetoPrimitiveRZ(Int_t px, Int_t py);
  virtual int  DistancetoPrimitiveTZ(Int_t px, Int_t py);

  virtual void Clear(const char* Opt = "")       ; // **MENU**
  virtual void Print(const char* Opt = "") const ; // **MENU**

  ClassDef(TTrkVisNode,0)
};


#endif
