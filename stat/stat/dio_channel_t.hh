//-----------------------------------------------------------------------------
// example : DIO  background
//-----------------------------------------------------------------------------
#ifndef __stntuple_sens_dio_channel_t_hh__
#define __stntuple_sens_dio_channel_t_hh__

#include "Stntuple/stat/pgaus_t.hh"
#include "Stntuple/stat/channel_t.hh"

namespace stntuple {
  class  dio_channel_t : public channel_t {
  public:
    double     fMean;
    double     fSigma;
    double     fVal;
    pgaus_t*   fLumi;			// explicitly - just one nuisanse parameter
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
    dio_channel_t(const char* Name,double Mean, double Sigma, int Debug = 0);
  
    virtual double  GetValue() ;

  };
}
#endif
