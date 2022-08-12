///////////////////////////////////////////////////////////////////////////////
// vis node displays one wedge
///////////////////////////////////////////////////////////////////////////////
#ifndef TEvdSimParticle_hh
#define TEvdSimParticle_hh

#include "Gtypes.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TPad.h"
#include "TLine.h"
#include "TEllipse.h"
#include "TParticlePDG.h"

namespace mu2e {
  class SimParticle;
  class StepPointMC;
};

namespace stntuple {
class TEvdSimParticle: public TObject {
public:
  int                       fNumber;
  const mu2e::SimParticle*  fSimp;
  const mu2e::StepPointMC*  fStep;

  TObjArray*                fListOfHits;
  TEllipse*                 fEllipse;
  TLine*                    fLineTZ;
  TParticlePDG*             fParticlePDG;
//-----------------------------------------------------------------------------
// constructors and destructor
//-----------------------------------------------------------------------------
  TEvdSimParticle();
  TEvdSimParticle(int Number, const mu2e::SimParticle* Simp, const mu2e::StepPointMC* Step); 

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
  virtual int DistancetoPrimitiveTZ (int px, int py);

  //  virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py)
//-----------------------------------------------------------------------------
// drawing functions
//-----------------------------------------------------------------------------
  virtual void  PaintXY  (Option_t* option = "");
  virtual void  PaintRZ  (Option_t* option = "");
  virtual void  PaintTZ  (Option_t* option = "");
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
