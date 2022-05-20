#ifndef __Stntuple_stat_pfixed_t_hh__
#define __Stntuple_stat_pfixed_t_hh__

#include "Stntuple/stat/parameter_t.hh"

namespace stntuple {
  class  pfixed_t : public parameter_t {
  public:
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------

    // this is good for gaussian, log-normal, uniform, fixed
    pfixed_t(const char* Name, double XMean, int Debug = 0);
    
    virtual double XMin();
    virtual double XMax();
    
    virtual void InitValue();

    virtual void Print(const Option_t* Opt) const ;
    
  };
}
#endif
