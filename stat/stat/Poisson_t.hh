//////////////////////////////////////////////////////////////
//class for a Poission PDF with systematics
//////////////////////////////////////////////////////////////
#ifndef __su2020_stat_Poisson_t_hh__
#define __su2020_stat_Poisson_t_hh__

#include <set>
#include "Math/ProbFunc.h"
#include "Math/DistFunc.h"
#include "TRandom3.h"
#include "TString.h"
#include "TNamed.h"

class TRandom3;
class TH1D;

#include "Stntuple/stat/var_t.hh"

namespace stntuple {
  class Poisson_t : public TNamed {
  public:
    // today's implementation assumes that the observables are external ???
    // var_t               obs_;
    std::vector<var_t*> mu_;
    std::vector<var_t*> sys_;
    int                 verbose_;
    int                 ngen_;
    int                 nmax_;
    TRandom3*           fRn;

    Poisson_t(TString name, int nmax, std::vector<var_t*> mu, std::vector<var_t*> sys = {});

    void   SetVerbose(int verbose) { verbose_ = verbose; }

    double GetMean       ();
    double GetNominalMean();
    double Eval     (int n);
    void   RandomSys     ();
    int    RandomSample  ();
    TH1D*  GeneratePDF   ();
    virtual void    Print(Option_t* Opt = "") const ;

  };

}

#endif
