//-----------------------------------------------------------------------------
// example : DIO  background
//-----------------------------------------------------------------------------
#ifndef __Stntuple_stat_channel_t_hh__
#define __Stntuple_stat_channel_t_hh__

#include "TRandom3.h"
#include "TNamed.h"
#include "TH1.h"
#include "TH2.h"

#include "Stntuple/stat/parameter_t.hh"

namespace stntuple {
class  channel_t : public TNamed {
public:
  parameter_t*  fBgr;
  TH1D*         fHistPDF;
  int           fDebug;
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  channel_t(const char* Name="undefined_channel", int Debug = 0);
  ~channel_t();

  void SetBgr(parameter_t* Bgr);

  TH1D* GetHistPDF() { return fHistPDF; }

  virtual double  GetValue();

};
}
#endif
