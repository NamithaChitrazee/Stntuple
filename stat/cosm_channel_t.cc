////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/cosm_channel_t.hh"

namespace stntuple {

  cosm_channel_t::cosm_channel_t(const char* Name, int Debug) : channel_t(Name, Debug) {
    fLumi           = nullptr;
  }

//-----------------------------------------------------------------------------
//  assume that all nuisanse parameters have been initalized
//  BGR \propto N(events)/lumi 
//-----------------------------------------------------------------------------
  double cosm_channel_t::GetValue() {

    fVal = fBgr->GetValue();
//-----------------------------------------------------------------------------
// cosmics: scale with fluctuated *inverse* luminosity - the lower is the luminosity,
// the longer is the data taking time
//-----------------------------------------------------------------------------
    if (fLumi) {
      fVal = fVal*(fLumi->InverseValue()/fLumi->InverseMean());
    }

    if (fDebug) {
      fHistPDF->Fill(fVal);
    }

    return fVal;
  }

}
