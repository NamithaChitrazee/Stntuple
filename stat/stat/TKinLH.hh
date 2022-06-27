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
#include "Stntuple/stat/TBelt.hh"
//-----------------------------------------------------------------------------
namespace stntuple {
class TKinLH : public TBelt {
public:
  struct Debug_t {
    int      fRun;
    int      fConstructBelt;
    int      fTestCoverage;
    double   fMuMin;
    double   fMuMax;
  } fDebug;

  struct Hist_t {
    TH1D*    fProb;
    TH1D*    fLlh;
    TH1D*    fLlhInterval;
    TH1D*    fInterval;
    TH1D*    fBeltLo;
    TH1D*    fBeltHi;
    TH1D*    fBeltNO;
    TH1D*    fBeltUL;
    TH1D*    fBeltNO1;
    // TH1D*    fSign[2];
    TGraph*  fCoverage;

    TH1F*    prob_sig;
    TH1F*    prob_bgr;
    
    TH1D*    log_lhs [MaxNx];
    TH1D*    log_lhb [MaxNx];
    TH1D*    log_lhr [MaxNx];
    TH1D*    log_lhrR[MaxNx];
    
    TH1D*    log_lhrR_1[MaxNx];         // uniformly normalized, 2-sided distributions
    TH1D*    sum_log_lhrR_1;            // sum of all .

                                        // these are not used and are filled only in debug mode
    TH1D*    gen_pbgr;               // generated momentum, background
    TH1D*    gen_psig;               // generated momentum, signal
  };

  struct Interval_t {
    double fLlhrMin;
    double fLlhrMax;
    double fProbTot;
  } fInterval;

  Hist_t   fHist;
  int      fColor;

  double   pmin;
  double   pmax;

  double   fMinLLHR;
  double   fMaxLLHR;
  
  int      fMode;                       // 0: background, 1:signal
  
  TRandom3 fRng;
                                        // signal and background kinematic probability distributions
                                        // start with 1D momentum distributions, but, in general, those
                                        // are the 1D probability distributions integrated over all kinematic
                                        // variables.
  TF1*     fSig;
  TF1*     fBgr;

  void*    fTestHist;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  TKinLH(const char* Name, double CL = 0.9, int Mode = 0, double PMin=103.6, double PMax = 104.9, int Debug = 0);
  ~TKinLH();

  int    init();
                                        // signal parameterization with the integral \int f(x)*dx = 1
  
  double static f_sig(double* X, double * P);
  double static f_bgr(double* X, double * P);
  
  double lh_bgr(double P);
  double lh_sig(double P);

  double bgr_mom();
  double sig_mom();

  virtual int  construct_interval(double MuB, double MuS, int NObs = -1);
  virtual int  construct_belt    (double MuB, double SMin, double SMax, int NPoints, int NObs = -1);
  virtual int  test_coverage(double MuB, double SMin, double SMax, int NPoints);
  
                                        // with the kinematic distributions in, NObs = -1 simply doesn't make sense
  
  int    run(int NObs, int NPe = 1000000);

  int    generate();

  int    save_hist(const char* Filename, const char* Opt = "append");
  int    read_hist(const char* Filename);
  
  void   make_belt_hist();

  virtual void Print(const char* Option_t) const;

  ClassDef(TKinLH,0)
};
}
#endif
