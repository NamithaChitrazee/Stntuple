#ifndef Stntuple_gui_TTrkVisNode_hh
#define Stntuple_gui_TTrkVisNode_hh

#include "Gtypes.h"
#include "TClonesArray.h"
#include "TArc.h"
//------------------------------------------------------------------------------
// this clause is necessary
//-----------------------------------------------------------------------------
#ifndef __CINT__
#include "Offline/RecoDataProducts/inc/KalRepPtrCollection.hh"
#include "Offline/TrackerGeom/inc/Tracker.hh"
#include "Offline/RecoDataProducts/inc/ComboHit.hh"
#include "Offline/RecoDataProducts/inc/TimeCluster.hh"
#include "Offline/MCDataProducts/inc/StrawDigiMC.hh"
#include "Offline/MCDataProducts/inc/SimParticle.hh"
#include "Offline/MCDataProducts/inc/StepPointMC.hh"

#else
namespace mu2e {
  class ComboHitCollection;
  class StrawDigiMCCollection;
  class KalRepPtrCollection;
  class Tracker;
};
#endif

#include "Stntuple/gui/TStnVisNode.hh"

class TStnTrackBlock;

namespace stntuple {
  class TEvdStrawTracker;
  class TEvdStrawHit;
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

  const mu2e::ComboHitCollection**             fComboHitColl;
  const mu2e::TimeClusterCollection**          fTimeClusterColl;  //
  const mu2e::StrawDigiMCCollection**          fStrawDigiMCColl; 
  const mu2e::KalRepPtrCollection**            fKalRepPtrColl;
  const mu2e::SimParticleCollection**          fSimpColl;
  const mu2e::StepPointMCCollection**          fSpmcColl;

  TStnTrackBlock*           fTrackBlock;
  Color_t                   fTrackColor;

  stntuple::TEvdStrawTracker* fTracker;

  TArc*                     fArc;
  const mu2e::TimeCluster*  fTimeCluster;

  Int_t                     fDisplayBackgroundHits;
  Int_t                     fTimeWindow;
  Int_t                     fPickMode;
  Int_t                     fUseStereoHits;
  double                    fEventTime;
		            
  TObjArray*                fListOfStrawHits;
  TObjArray*                fListOfTracks;
  TObjArray*                fListOfSimParticles;

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
  int        GetNHits  ()      { return fListOfStrawHits->GetEntriesFast(); }

  stntuple::TEvdStrawHit* GetHit  (int I) { return (stntuple::TEvdStrawHit*) fListOfStrawHits->At(I); }
  stntuple::TEvdTrack*    GetTrack(int I) { return (stntuple::TEvdTrack*)    fListOfTracks->At(I); }

  stntuple::TEvdSimParticle* GetSimParticle(int I) { return (stntuple::TEvdSimParticle*)  fListOfSimParticles->At(I); }

  const mu2e::ComboHitCollection* GetComboHitColl() { 
    return *fComboHitColl; 
  }

  // const mu2e::StrawHitFlagCollection* GetStrawHitFlagColl() { 
  //   return *fStrawHitFlagColl;
  // }

  int DisplayBackgroundHits() { return fDisplayBackgroundHits; }
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  void  SetKalRepPtrColl(const mu2e::KalRepPtrCollection** Coll) {
    fKalRepPtrColl = Coll;
  }

  void  SetListOfTracks(TObjArray* List) { fListOfTracks = List; }
  void  SetTrackColor  (Color_t      c ) { fTrackColor   = c   ; }

  void SetComboHitColl(const mu2e::ComboHitCollection** Coll) { 
    fComboHitColl = Coll;
  }

  // void SetStrawHitFlagColl(const mu2e::StrawHitFlagCollection** Coll) { 
  //   fStrawHitFlagColl = Coll;
  // }

  void SetStrawDigiMCColl(const mu2e::StrawDigiMCCollection** Coll) { 
    fStrawDigiMCColl = Coll;
  }

  void SetSimParticleColl(const mu2e::SimParticleCollection** Coll) { 
    fSimpColl = Coll;
  }

  void SetSpmcColl(const mu2e::StepPointMCCollection** Coll) { 
    fSpmcColl = Coll;
  }

  void SetTimeClusterColl(const mu2e::TimeClusterCollection** Coll) { 
    fTimeClusterColl = Coll;
  }

  void  SetPickMode   (Int_t Mode) { fPickMode    = Mode; }

  void  SetDisplayBackgroundHits(Int_t Mode) { fDisplayBackgroundHits = Mode; }
//-----------------------------------------------------------------------------
// overloaded methods of TVisNode
//-----------------------------------------------------------------------------
  virtual int   InitEvent();
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  virtual void  PaintXY (Option_t* option = "");
  virtual void  PaintRZ (Option_t* option = "");

  virtual Int_t DistancetoPrimitiveXY(Int_t px, Int_t py);
  virtual Int_t DistancetoPrimitiveRZ(Int_t px, Int_t py);

  //  virtual void   Clear(const char* Opt = "")       ; // **MENU**
  //  virtual void   Print(const char* Opt = "") const ; // **MENU**

  ClassDef(TTrkVisNode,0)
};


#endif
