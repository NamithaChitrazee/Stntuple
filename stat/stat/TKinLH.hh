///////////////////////////////////////////////////////////////////////////////
// fMode = 0: sample background momentum spectrum
//       = 1: sample signal     momentum spectrum
//
// initially, I forgot the fact that the signal and background
// means over the [103.6,104.9] are very close
// - the plan unchanged: normalize the weights properly,
//   assign each pseudoexperiment event the weight 
//   of the 2D probability density to get the values of pmean and sigmap
//   from the very beginning, consider the background as consisting of
//   two components: a uniform one and the DIO.
///////////////////////////////////////////////////////////////////////////////
#ifndef __Stntuple_stat_TKinLH_hh__
#define __Stntuple_stat_TKinLH_hh__

#include "TRandom3.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TF1.h"

#include "TNamed.h"
#include "TFile.h"

#include "Stntuple/val/stntuple_val_functions.hh"
//-----------------------------------------------------------------------------
namespace stntuple {
class TKinLH : public TNamed {
public:
  struct Hist_t {
    TH1F*    prob_sig;
    TH1F*    prob_bgr;
    
    TH1D*    llhs;
    TH1D*    llhb;
    TH1D*    llhr;
    TH1D*    llhrR;
    
    TH1D*    gen_pbgr;                  // generated momentum, background
    TH1D*    gen_psig;                  // generated momentum, signal
    
    TH2D*    sig_vs_p[20];
  };

  Hist_t   fHist;

  double   pmin;
  double   pmax;
  int      initialized;

  int      fDebug;
  int      fMode;   // 0: background, 1:signal
  
  TRandom3 fRng;

  TF1*     fSig;

  void*    fTestHist;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  TKinLH(int Mode, const char* Name, double PMin=103.6, double PMax = 104.9, int Debug = 0);
  ~TKinLH();

  int    init();
                                        // signal parameterization with the integral \int f(x)*dx = 1
  
  double static f_sig(double* X, double * P);
  
  double lh_bgr(double P);
  double lh_sig(double P);

  double bgr_mom();
  double sig_mom();
                                        // with the kinematic distributions in, NObs = -1 simply doesn't make sense
  
  int    run(int NObs, int NPe = 1000000);

  int    generate();

  int    save_hist(const char* Filename, const char* Opt = "append");

  ClassDef(TKinLH,0)
};
}
#endif
