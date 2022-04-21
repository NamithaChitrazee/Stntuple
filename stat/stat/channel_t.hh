//-----------------------------------------------------------------------------
// example : DIO  background
//-----------------------------------------------------------------------------
#ifndef __Stntuple_stat_channel_t_hh__
#define __Stntuple_stat_channel_t_hh__

#include "TRandom3.h"
#include "TNamed.h"
#include "TH1.h"
#include "TH2.h"

namespace stntuple {
class  channel_t : public TNamed {
public:
  TH1D*     fHistPDF;
  int       fDebug;
  TRandom3* fRn;
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  channel_t(const char* Name, int Debug = 0);

  TH1D* GetHistPDF() { return fHistPDF; }

  virtual double  GetValue();

};
}
#endif
