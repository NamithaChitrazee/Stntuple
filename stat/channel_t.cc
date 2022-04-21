////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/channel_t.hh"

namespace stntuple {

  channel_t::channel_t(const char* Name): TNamed(Name,Name) {
    fHistPDF        = nullptr;
    if (fDebug) {
      //      fHistPDF = new TH1D(Form("h_%s",Name),"par",1000,fMean-10*Sigma,fMean+10*Sigma);
    }
  }

//-----------------------------------------------------------------------------
//  assume that all nuisanse parameters have been initalized
//-----------------------------------------------------------------------------
  double channel_t::GetValue() {
    return -1; //fVal;
  }

}
