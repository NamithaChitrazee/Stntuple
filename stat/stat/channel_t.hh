//-----------------------------------------------------------------------------
// example : DIO  background
//-----------------------------------------------------------------------------
#ifndef __Stntuple_stat_channel_t_hh__
#define __Stntuple_stat_channel_t_hh__

#include "TNamed.h"
#include "TH1.h"
#include "TH2.h"

namespace stntuple {
class  channel_t : public TNamed {
public:
  double   fMeanBgr;
  TH1D*    fHistPDF;
  int      fDebug;
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  channel_t(const char* Name);

  TH1D* GetHistPDF() { return fHistPDF; }

  double  GetValue();

  double  GetMeanBgr() { return fMeanBgr; }

};
}
#endif
