///////////////////////////////////////////////////////////////////////////////
// vis node displays one wedge
///////////////////////////////////////////////////////////////////////////////
#ifndef TEvdTimeCluster_hh
#define TEvdTimeCluster_hh

#include "Gtypes.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TBox.h"

namespace mu2e {
  class TimeCluster;
  class ComboHit;
}

namespace stntuple {

class TEvdTrkStrawHit;

class TEvdTimeCluster: public TObject {
public:
  
protected:
  int                       fNumber;
  const mu2e::TimeCluster*  fTimeCluster;

  TObjArray*                fListOfHits;   // list of combo hits
  float                     fTMin;
  float                     fTMax;
  float                     fZMin;
  float                     fZMax;
  int                       fColor;
  TBox*                     fBox;
public:
//-----------------------------------------------------------------------------
// constructors and destructor
//-----------------------------------------------------------------------------
  TEvdTimeCluster();

  TEvdTimeCluster(int Number, const mu2e::TimeCluster* aTimeCluster,
		  float TMin, float TMax, float ZMin, float ZMax);

  virtual ~TEvdTimeCluster();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
  const mu2e::TimeCluster* TimeCluster() const { return fTimeCluster; }
  float                    TMin       () const { return fTMin; }
  float                    TMax       () const { return fTMax; }
  float                    ZMin       () const { return fZMin; }
  float                    ZMax       () const { return fZMax; }
  TBox*                    Box        () const { return fBox;  }
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
					// Type: "ch" or "sh"
  int                      HitOK(const mu2e::ComboHit* Hit, const char* Type);
//-----------------------------------------------------------------------------
// drawing functions
//-----------------------------------------------------------------------------
  virtual void  PaintTZ  (Option_t* option = "");
  virtual Int_t DistancetoPrimitiveTZ (Int_t px, Int_t py);
  //  virtual Int_t DistancetoPrimitiveCal(Int_t px, Int_t py);
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  virtual void  Paint(Option_t* Opt = "");
  virtual void  Clear(Option_t* Opt = "");
  virtual void  Print(Option_t* Opt = "") const ; // **MENU**

  virtual int   DistancetoPrimitive(Int_t px, Int_t py);
  virtual void  ExecuteEvent       (int Event, int Px, int Py);	


  ClassDef(stntuple::TEvdTimeCluster,0)
};

}
#endif
