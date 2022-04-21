#ifndef __su2020_sens_pflat_t_hh__
#define __su2020_sens_pflat_t_hh__

#include "Stntuple/stat/pflat_t.hh"

namespace stntuple {
class  pgaus_t : public parameter_t {
public:
  double   fMin;
  double   fMax;
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  // this is good for gaussian, log-normal, uniform
  pflat_t(const char* Name, int Type, double Min, double Max, int Debug);

#ifndef __su2020_sens_parameter_t_hh__
#define __su2020_sens_parameter_t_hh__

#include "TNamed.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"

namespace stntuple {
  class  parameter_t : public TNamed {
  public:
    double    fValue;
    TRandom3* fRn;
    TH1D*     fHistPDF;			// PDF distribution
    int       fDebug;
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------

    parameter_t(const char* Name, int Debug = 0);
    ~parameter_t();

#ifndef __su2020_sens_parameter_t_hh__
#define __su2020_sens_parameter_t_hh__

#include "TNamed.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom3.h"

namespace stntuple {
  class  pflat_t : public TNamed {
  public:
    double    fValue;
    double    fMin;
    double    fMax;
    TRandom3* fRn;
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
    pflat_t(const char* Name, int Debug = 0);
    ~pflat_t();

    virtual void  InitValue();

    virtual void Print(const Option_t* Opt) const ;

  };
}
#endif
