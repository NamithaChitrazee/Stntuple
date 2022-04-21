#ifndef __su2020_sens_pflat_t_hh__
#define __su2020_sens_pflat_t_hh__

#include "Stntuple/stat/pflat_t.hh"

namespace stntuple {
class  pgaus_t : public parameter_t {
public:
  double   fMin;
  double   fMax;
  double   fVal;	                // initialized value 
  TH1D*    fHistPdf;			// PDF distribution
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  // this is good for gaussian, log-normal, uniform
  pflat_t(const char* Name, int Type, double Min, double Max);

  virtual double  GetValue();

  virtual void Print(const Option_t* Opt) const ;

};
}
#endif
