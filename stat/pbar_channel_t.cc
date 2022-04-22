////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/pbar_channel_t.hh"

namespace stntuple {

  pbar_channel_t::pbar_channel_t(const char* Name, int Debug) : channel_t(Name, Debug) {
    fLumi           = nullptr;
  }

//-----------------------------------------------------------------------------
//  assume that all nuisanse parameters have been initalized
//  BGR \propto N(events)/lumi 
//-----------------------------------------------------------------------------
  double pbar_channel_t::GetValue() {

    fVal = fBgr->GetValue();
					// scale with fluctuated luminosity:
    if (fLumi) {
      fVal = fVal*fLumi->GetValue()/fLumi->Mean();
    }

    if (fDebug) {
      fHistPDF->Fill(fVal);
    }

    return fVal;
  }

}
