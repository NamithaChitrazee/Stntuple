#ifndef __su2020_sens_pgaus_t_hh__
#define __su2020_sens_pgaus_t_hh__

#include "Stntuple/stat/parameter_t.hh"

namespace stntuple {
  class  pgaus_t : public parameter_t {
  public:
    double   fMean;
    double   fSigma;
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
    // this is good for gaussian, log-normal, uniform
    pgaus_t(const char* Name, double Mean, double Sigma, int Debug = 0);

    double Mean       () { return fMean;    }
    double InverseMean() { return 1./fMean; }

    virtual double XMin();
    virtual double XMax();
    
    virtual void   InitValue();
    
    virtual void   Print(const Option_t* Opt) const ;
    
  };
}
#endif
