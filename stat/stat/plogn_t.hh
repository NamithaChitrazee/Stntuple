#ifndef __Stntuple_stat_plogn_t_hh__
#define __Stntuple_stat_plogn_t_hh__

#include "Stntuple/stat/parameter_t.hh"

namespace stntuple {
  class  plogn_t : public parameter_t {
  public:
    double   fSigmaOverMean;
    double   fMu;			        // parameters of teh lognormal dist
    double   fSigma;
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
    // this is good for gaussian, log-normal, uniform
    plogn_t(const char* Name, double Mean, double SigmaOverMean, int Debug = 0);
    
    double Mu         () { return fMu     ; }
    double Sigma      () { return fSigma  ; }

    virtual double XMin();
    virtual double XMax();
    
    virtual void   InitValue();
    
    virtual void   Print(const Option_t* Opt) const ;
    
  };
}
#endif
