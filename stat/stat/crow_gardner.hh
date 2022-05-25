///////////////////////////////////////////////////////////////////////////////
#ifndef __Stntuple_stat_crow_gardner__
#define __Stntuple_stat_crow_gardner__

#include "TNamed.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THStack.h"
#include "TRandom3.h"
#include "TGraph.h"

namespace stntuple {
//-----------------------------------------------------------------------------
class crow_gardner: public TNamed {
public:

  enum {
    MaxNx =   50
  };

  struct Belt_t {
    double fSMin;
    double fSMax;
    double fDy;
    double fSign[MaxNx][2];      // fBelt.Sign[ix][0] = SMin[ix], fBelt.fSign[ix][1] = SMax[ix]
  } fBelt;

  struct Debug_t {
    int    fConstructBelt;
    int    fUpperLimit;
    int    fTestCoverage;
    double fMuMin;
    double fMuMax;
  } fDebug;
  
  struct Hist_t {
    TH1D*    fProb;
    TH1D*    fLh;
    TH1D*    fInterval;
    THStack* fBelt;
    TH1D*    fBeltLo;
    TH1D*    fBeltHi;
    TH1D*    fSign[2];
    TGraph*  fCoverage;
  };

  Hist_t fHist;

  int    fType;                      // default:0 if=1, try to force monotonic left edge
  double fCL;
  double fSump;

  double fMean;
  double fMuB;
  double fMuS;
  
  int    fIxMin;
  int    fIxMax;

  long int fNExp;

  TRandom3 fRn;

  double fProb     [MaxNx];
  double fCProb    [MaxNx];
  double fFactorial[MaxNx];

  void SetNExp(long int NExp) { fNExp = NExp; }

  crow_gardner(const char* Name, double Mean, double CL = -1, int TYpe = 0);

                                        // 'N' - number of measured events
  
  int init_poisson_dist(double MuB, double MuS, int NObs = -1);
  
                                        // in presence of background, mu = mus+mub
  
  int construct_interval(double MuB, double MuS);
  int construct_belt    (double MuB, double SMin, double SMax, int NPoints);

  int  make_prob_hist();
  void make_belt_hist();

  int test_coverage(double MuB, double SMin, double SMax, int NPoints);

  ClassDef(crow_gardner,0)
};

}
#endif
