//-----------------------------------------------------------------------------
// example : DIO  background
//-----------------------------------------------------------------------------
#ifndef __Stntuple_stat_channel_t_hh__
#define __Stntuple_stat_channel_t_hh__

#include "TRandom3.h"
#include "TNamed.h"
#include "TH1.h"
#include "TH2.h"
#include "TObjArray.h"

#include "Stntuple/stat/parameter_t.hh"

namespace stntuple {
  class  channel_t : public TNamed {
  public:
    parameter_t*  fProcess;
    TH1D*         fHistPDF;
    int           fDebug;
    int           fSignal;        // normally - zero, for signal = 1
    TObjArray*    fListOfPAdd;    // list of parameters which need to be added (1+p1+p2...)
// -----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
    channel_t(const char* Name="undefined_channel", int Debug = 0);
    ~channel_t();
					// returns 1 for the signal channel
    int            Signal() { return fSignal; }

    parameter_t*   Process() { return fProcess; }

    TH1D*          GetHistPDF() { return fHistPDF; }

    double         GetAddCorr();
    
    virtual double GetValue();
    
    void           SetSignal(int Signal) { fSignal = Signal; }
    
    void           SetProcess(parameter_t* Process);
    
    void           AddPAdd(parameter_t* Par) { fListOfPAdd->Add(Par); }
  };
}
#endif
