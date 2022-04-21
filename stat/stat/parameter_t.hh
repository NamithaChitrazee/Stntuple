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

    virtual void  InitValue();
    
    double  GetValue() { return fValue; };

    TH1D*  GetHistPDF() { return fHistPDF; };

    void   SetDebug(int Value) { fDebug = Value; }
    
    virtual void Print(const Option_t* Opt) const ;

    ClassDef(stntuple::parameter_t,0)
    
  };
}

#endif
