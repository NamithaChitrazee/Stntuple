///////////////////////////////////////////////////////////////////////////////
// vis node displays one wedge
///////////////////////////////////////////////////////////////////////////////
#ifndef TEvdStrawTracker_hh
#define TEvdStrawTracker_hh

#include "Gtypes.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TPad.h"
#include "TArc.h"

#ifndef __CINT__
#include "Offline/RecoDataProducts/inc/StrawHit.hh"
#include "Offline/RecoDataProducts/inc/StrawHitFlag.hh"
#include "Offline/TrackerGeom/inc/Tracker.hh"
#else
namespace mu2e {
  class StrawHitCollection;
  class StrawHitFlagCollection;
  class TTracker;
};
#endif

namespace stntuple {

class TEvdStation;

class TEvdStrawTracker: public TObject {
public:
  
protected:

  int        fNStations;
  TObjArray* fListOfStations;

  const mu2e::Tracker*  fTracker;

public:
//-----------------------------------------------------------------------------
// constructors and destructor
//-----------------------------------------------------------------------------
  TEvdStrawTracker(const mu2e::Tracker* Tracker = NULL);
  //  TEvdStrawTracker(const char* Name); 

  virtual ~TEvdStrawTracker();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  int          NStations     () { return fNStations;      }
  TObjArray*   ListOfStations() { return fListOfStations; }

  TEvdStation* Station  (int I) { 
    return (TEvdStation*) fListOfStations->UncheckedAt(I); 
  }
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  virtual void  Paint   (Option_t* option = "");

  void  PaintXY   (Option_t* option = "");
  void  PaintRZ   (Option_t* option = "");
  void  PaintVST  (Option_t* option = "");

  //  virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py);

  virtual Int_t DistancetoPrimitive   (Int_t px, Int_t py);
  virtual Int_t DistancetoPrimitiveXY (Int_t px, Int_t py);
  virtual Int_t DistancetoPrimitiveRZ (Int_t px, Int_t py);
  virtual Int_t DistancetoPrimitiveVST(Int_t px, Int_t py);

  //  virtual void   Print(const char* Opt = "") const ; // **MENU**

  ClassDef(stntuple::TEvdStrawTracker,0)
};
}

#endif
