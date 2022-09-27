#ifndef Stntuple_gui_TTrkVisNode_hh
#define Stntuple_gui_TTrkVisNode_hh

#include "Gtypes.h"
#include "TClonesArray.h"
#include "TArc.h"
//------------------------------------------------------------------------------
// this clause is necessary
//-----------------------------------------------------------------------------
// #ifndef __CINT__
#include "Offline/RecoDataProducts/inc/KalRepPtrCollection.hh"
#include "Offline/TrackerGeom/inc/Tracker.hh"
#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/StrawDigi.hh"
#include "Offline/RecoDataProducts/inc/StrawHit.hh"
#include "Offline/RecoDataProducts/inc/TimeCluster.hh"
#include "Offline/MCDataProducts/inc/StrawDigiMC.hh"
#include "Offline/MCDataProducts/inc/SimParticle.hh"
#include "Offline/MCDataProducts/inc/StepPointMC.hh"

// // #else
// namespace mu2e {
//   class ComboHitCollection;
//   class StrawDigiMCCollection;
//   class KalRepPtrCollection;
//   class Tracker;
// };
// #endif

#include "Stntuple/gui/TStnVisNode.hh"

class TStnTrackBlock;
class TSimpBlock;

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

  const mu2e::ComboHitCollection**             fCComboHitColl;     // several SH per CH
  const mu2e::ComboHitCollection**             fSComboHitColl;     // one     SH per CH
  const mu2e::TimeClusterCollection**          fTimeClusterColl;   //
  const mu2e::StrawDigiCollection**            fSdColl;		   // straw digi coll
  const mu2e::StrawDigiMCCollection**          fSdmcColl;          // straw digi MC coll
  const mu2e::StrawHitCollection**             fShColl;            // straw hit coll
  const mu2e::StrawDigiADCWaveformCollection** fSwColl;		   // straw digi waveform coll
  const mu2e::KalRepPtrCollection**            fKalRepPtrColl;
  const mu2e::SimParticleCollection**          fSimpColl;
  const mu2e::StepPointMCCollection**          fSpmcColl;

  TStnTrackBlock*           fTrackBlock;
  Color_t                   fTrackColor;

  stntuple::TEvdStrawTracker* fTracker;

  TArc*                     fArc;
  const mu2e::TimeCluster*  fTimeCluster;

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

  const mu2e::ComboHitCollection* GetCComboHitColl() { return *fCComboHitColl; }
  const mu2e::ComboHitCollection* GetSComboHitColl() { return *fSComboHitColl; }

  // const mu2e::StrawHitFlagCollection* GetStrawHitFlagColl() { 
  //   return *fStrawHitFlagColl;
  // }

//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  void  SetKalRepPtrColl(const mu2e::KalRepPtrCollection** Coll) {
    fKalRepPtrColl = Coll;
  }

  //  void  SetInitSimpBlock(StntupleInitSimpBlock* InitBlock) { fInitSimpBlock = InitBlock; }

  void  SetSimpBlock   (TSimpBlock* Block) { fSimpBlock    = Block; }
  void  SetListOfTracks(TObjArray*  List ) { fListOfTracks = List ; }
  void  SetTrackColor  (Color_t     Color) { fTrackColor   = Color; }

  void SetCComboHitColl(const mu2e::ComboHitCollection** Coll) { 
    fCComboHitColl = Coll;
  }

  void SetSComboHitColl(const mu2e::ComboHitCollection** Coll) { 
    fSComboHitColl = Coll;
  }

  void SetShColl(const mu2e::StrawHitCollection** Coll) { 
    fShColl = Coll;
  }

  void SetSwColl(const mu2e::StrawDigiADCWaveformCollection** Coll) { 
    fSwColl = Coll;
  }

  // void SetStrawHitFlagColl(const mu2e::StrawHitFlagCollection** Coll) { 
  //   fStrawHitFlagColl = Coll;
  // }

  void SetSdmcColl(const mu2e::StrawDigiMCCollection** Coll) { 
    fSdmcColl = Coll;
  }

  void SetSimpColl(const mu2e::SimParticleCollection** Coll) { 
    fSimpColl = Coll;
  }

  void SetSpmcColl(const mu2e::StepPointMCCollection** Coll) { 
    fSpmcColl = Coll;
  }

  void SetTimeClusterColl(const mu2e::TimeClusterCollection** Coll) { 
    fTimeClusterColl = Coll;
  }

  void  SetPickMode   (Int_t Mode) { fPickMode    = Mode; }
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
