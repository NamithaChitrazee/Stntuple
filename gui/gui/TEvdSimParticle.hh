///////////////////////////////////////////////////////////////////////////////
// vis node displays one wedge
///////////////////////////////////////////////////////////////////////////////
#ifndef TEvdSimParticle_hh
#define TEvdSimParticle_hh

#include "Gtypes.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TPad.h"
#include "TArc.h"
#include "TEllipse.h"

namespace mu2e {
  class SimParticle;
};

namespace stntuple {
class TEvdSimParticle: public TObject {
public:
  int                       fNumber;
  const mu2e::SimParticle*  fSimp;

  TObjArray*                fListOfHits;
  TEllipse*                 fEllipse;
//-----------------------------------------------------------------------------
// constructors and destructor
//-----------------------------------------------------------------------------
  TEvdSimParticle();
  TEvdSimParticle(int Number, const mu2e::SimParticle* Simp); 

  virtual ~TEvdSimParticle();
//-----------------------------------------------------------------------------
// accessors
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// modifiers
//-----------------------------------------------------------------------------
  virtual int DistancetoPrimitive   (int px, int py);
  virtual int DistancetoPrimitiveXY (int px, int py);
  virtual int DistancetoPrimitiveRZ (int px, int py);

  //  virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py)
//-----------------------------------------------------------------------------
// drawing functions
//-----------------------------------------------------------------------------
  virtual void  PaintXY  (Option_t* option = "");
  virtual void  PaintRZ  (Option_t* option = "");
//-----------------------------------------------------------------------------
// overloaded methods of TObject
//-----------------------------------------------------------------------------
  virtual void  Paint(Option_t* Opt = "");
  virtual void  Clear(Option_t* Opt = "");
  virtual void  Print(Option_t* Opt = "") const ; // **MENU**

  ClassDef(stntuple::TEvdSimParticle,0)
};

}

#endif
