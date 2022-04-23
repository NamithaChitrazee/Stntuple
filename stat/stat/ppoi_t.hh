#ifndef __Stntuple_stat_ppoi_t_hh__
#define __Stntuple_stat_ppoi_t_hh__

#include "Stntuple/stat/parameter_t.hh"

namespace stntuple {
  class  ppoi_t : public parameter_t {
  public:
    double   fMean;
    double   fSigma;
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
    // this is good for gaussian, log-normal, uniform
    ppoi_t(const char* Name, double Mean, double Sigma, int Debug = 0);

    double Mean       () { return fMean;    }
    double InverseMean() { return 1./fMean; }

    virtual double XMin();
    virtual double XMax();
    
    virtual void   InitValue();
    
    virtual void   Print(const Option_t* Opt) const ;
    
  };
}
#endif
