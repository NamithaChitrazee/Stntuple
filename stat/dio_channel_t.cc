////////////////////////////////////////////////////////////////////////////////
// verbose = 0: default
//         = 1: fill histogram
//         = 2: print value
//         > 2: more detailed printout
////////////////////////////////////////////////////////////////////////////////
#include "Stntuple/stat/dio_channel_t.hh"

namespace stntuple {

  dio_channel_t::dio_channel_t(const char* Name, int Debug) : channel_t(Name, Debug) {
    fLumi           = nullptr;
  }

//-----------------------------------------------------------------------------
//  assume that all nuisanse and non-nuisance parameters have been initalized
//-----------------------------------------------------------------------------
  double dio_channel_t::GetValue() {

    fVal = fProcess->GetValue();
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
