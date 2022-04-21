//
#ifndef __murat_alg_Stntuple_TStatModel__
#define __murat_alg_Stntuple_TStatModel__

#include "TRandom3.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TNamed.h"
#include  "TGraph.h"

namespace stntuple {
class TStatModel : public TNamed {
public:

  // a list of channels, which contributions may depend on parameters

  
  TObjArray* fListOfChannels;

  // list of nuisance parameters, contribution of each channel
  // may depend on parameters

  TObjArray* fListOfParameters;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  TStatModel(const char* Name,
		   double      CL,
		   int         DebugLevel = 0);
  
  ~TStatModel();

  void   SetDebugLevel  (int Level ) { fDebugLevel = Level; };
  
  
  ClassDef(TStatModel,0)
};
};
#endif
