//-----------------------------------------------------------------------------
// mu --> e signal channel
//-----------------------------------------------------------------------------
#ifndef __Stntuple_stat_mu2e_channel_t_hh__
#define __Stntuple_stat_mu2e_channel_t_hh__

#include "Stntuple/stat/pgaus_t.hh"
#include "Stntuple/stat/plogn_t.hh"

#include "Stntuple/stat/channel_t.hh"

namespace stntuple {
  class  mu2e_channel_t : public channel_t {
  public:
    double     fMean;
    double     fSigma;
    double     fVal;
    
    pgaus_t*   fLumi;			// explicitly - just one nuisanse parameter
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
    mu2e_channel_t(const char* Name, int Debug = 0);
  
    virtual double  GetValue() ;

  };
}
#endif
