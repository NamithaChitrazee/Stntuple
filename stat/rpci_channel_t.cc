////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/rpci_channel_t.hh"

namespace stntuple {

  rpci_channel_t::rpci_channel_t(const char* Name, int Debug) : channel_t(Name, Debug) {
    fLumi           = nullptr;
  }

//-----------------------------------------------------------------------------
//  assume that all nuisanse parameters have been initalized
//  BGR \propto N(events)/lumi 
//-----------------------------------------------------------------------------
  double rpci_channel_t::GetValue() {

    fVal = fBgr->GetValue();
					// scale with fluctuated luminosity:
    if (fLumi) {
      fVal = fVal*fLumi->GetValue()/fLumi->Mean();
    }
					// additive smearing
					// expect addcorr to be close to 1
    double add_corr = GetAddCorr();
    fVal = fVal*add_corr;

    if (fDebug) {
      fHistPDF->Fill(fVal);
    }

    return fVal;
  }

}
